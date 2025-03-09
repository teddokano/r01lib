/** NXP Analog Front End class library for MCX
 *
 *  @author  Tedd OKANO
 *
 *  Copyright: 2023 - 2025 Tedd OKANO
 *  Released under the MIT license
 */

#include	"AFE_NXP.h"
#include	"r01lib.h"
#include	<math.h>

using enum	NAFE13388_Base::Register16;
using enum	NAFE13388_Base::Register24;

double	AFE_base::delay_accuracy	= 1.1;

/* AFE_base class ******************************************/

AFE_base::AFE_base( SPI& spi, int nINT, int DRDY, int SYN, int nRESET ) : 
	SPI_for_AFE( spi ), enabled_channels( 0 ), pin_nINT( nINT ), pin_DRDY( DRDY ), pin_SYN( SYN, 1 ), pin_nRESET( nRESET, 1 )
{
}

AFE_base::~AFE_base()
{
}

void AFE_base::begin( void )
{
	reset();
	boot();	
}

template<> 
int32_t AFE_base::read( int ch, float delay )
{
	if ( delay == INFINITY )
		delay	= ch_delay[ ch ] * delay_accuracy;
	
	start_and_delay( ch, delay );
	return adc_read( ch );
};

template<>
double AFE_base::read( int ch, float delay )
{
	return read<int32_t>( ch, delay ) * coeff_uV[ ch ];
};

void AFE_base::start_and_delay( int ch, float delay )
{
	if ( delay >= 0.0 )
	{
		start( ch );
		wait( delay );
	}
}

int AFE_base::bit_count( uint32_t value )
{
	constexpr int	bit_length	= 32;
	int				count	= 0;
		
	for ( int i = 0; i < bit_length; i++ ) {
		if ( value & (0x1 << i) )
			count++;
	}
	
	return count;
}


/* NAFE13388_Base class ******************************************/

NAFE13388_Base::NAFE13388_Base( SPI& spi, int nINT, int DRDY, int SYN, int nRESET ) 
	: AFE_base( spi, nINT, DRDY, SYN, nRESET )
{
}

NAFE13388_Base::~NAFE13388_Base()
{
}

void NAFE13388_Base::boot( void )
{
	command( CMD_ABORT ); 
	reg( GPIO_CONFIG0, 0x0000 );
	reg( GPIO_CONFIG1, 0x0000 );
	reg( GPIO_CONFIG2, 0x0000 );
	reg( GPO_DATA,     0x0000 );
	reg( GPI_DATA,     0x0000 );
	wait( 0.001 );
	
	reg( SYS_CONFIG0,  0x0010 );
	wait( 0.001 );
}

void NAFE13388_Base::reset( bool hardware_reset )
{
	if ( hardware_reset )
	{
		pin_nRESET	= 0;
		wait( 0.001 );
		pin_nRESET	= 1;
	}
	else
	{
		command( CMD_RESET ); 
	}
	
	constexpr uint16_t	CHIP_READY	= 1 << 13;
	constexpr auto		RETRY		= 10;
	
	for ( auto i = 0; i < RETRY; i++ )
	{
		wait( 0.003 );
		if ( reg( SYS_STATUS0 ) & CHIP_READY )
			return;
	}
	
	panic( "NAFE13388 couldn't get ready. Check power supply or pin conections\r\n" );
}

void NAFE13388_Base::logical_ch_config( int ch, const uint16_t (&cc)[ 4 ] )
{	
	constexpr double	pga_gain[]	= { 0.2, 0.4, 0.8, 1, 2, 4, 8, 16 };
	
	command( ch );
	
	for ( auto i = 0; i < 4; i++ )
		reg( CH_CONFIG0 + i, cc[ i ] );
	
	const uint16_t	setbit	= 0x1 << ch;
	const uint16_t	bits	= bit_op( CH_CONFIG4, ~setbit, setbit );
	
	enabled_channels	= bit_count( bits );
			
	if ( cc[ 0 ] & 0x0010 )
		coeff_uV[ ch ]	= ((10.0 / (double)(1L << 24)) / pga_gain[ (cc[ 0 ] >> 5) & 0x7 ]) * 1e6;
	else
		coeff_uV[ ch ]	= (4.0 / (double)(1L << 24)) * 1e6;
	
	ch_delay[ ch ]	= calc_delay( ch );
}

double NAFE13388_Base::calc_delay( int ch )
{
	constexpr static double	data_rates[]	= {	   288000, 192000, 144000, 96000, 72000, 48000, 36000, 24000, 
													18000,  12000,   9000,  6000,  4500,  3000,  2250,  1125, 
													 562.5,    400,    300,   200,   100,    60,    50,    30, 
														25,     20,     15,    10,   7.5, 						};
	constexpr static uint16_t	delays[]	= {		0,   2,   4,   6,   8,  10,   12,  14, 
												   16,  18,  20,  28,  38,  40,   42,  56, 
												   64,  76,  90, 128, 154, 178, 204, 224, 
												  256, 358, 512, 716, 
												  1024, 1664, 3276, 7680, 19200, 23040, };
	
	uint16_t ch_config1	= reg( CH_CONFIG1 );
	uint16_t ch_config2	= reg( CH_CONFIG2 );
	
	uint8_t		adc_data_rate		= (ch_config1 >>  3) & 0x001F;
	uint8_t		adc_sinc			= (ch_config1 >>  0) & 0x0007;
	uint8_t		ch_delay			= (ch_config2 >> 10) & 0x003F;
	bool		adc_normal_setting	= (ch_config2 >>  9) & 0x0001;
	bool		ch_chop				= (ch_config2 >>  7) & 0x0001;
	
	double		base_freq			= data_rates[ adc_data_rate ];
	double		delay_setting		= delays[ ch_delay ] / 4608000.00;
	
	if ( (28 < adc_data_rate) || (4 < adc_sinc) || ((adc_data_rate < 12) && (adc_sinc)) )
		return 0.00;
	
	if ( !adc_normal_setting  )
		base_freq	/= (adc_sinc + 1);
	
	if ( ch_chop )
		base_freq	/= 2;
	
#if 0
	printf( "base_freq = %lf\r\n", base_freq );
	printf( "delay_setting = %lf\r\n", delay_setting  );
	printf( "total delay = %lf\r\n", (1 / base_freq) + delay_setting  );
#endif
	
	return (1 / base_freq) + delay_setting;
}


void NAFE13388_Base::logical_ch_config( int ch, uint16_t cc0, uint16_t cc1, uint16_t cc2, uint16_t cc3 )
{	
	const ch_setting_t	tmp_ch_config	= { cc0, cc1, cc2, cc3 };
	logical_ch_config( ch, tmp_ch_config );
}

void NAFE13388_Base::logical_ch_disable( int ch )
{	
	const uint16_t	clearingbit	= 0x1 << ch;
	const uint16_t	bits		= bit_op( CH_CONFIG4, ~clearingbit, ~clearingbit );

	enabled_channels	= bit_count( bits );
}

int32_t NAFE13388_Base::adc_read( int ch )
{
	return reg( CH_DATA0 + ch );
}

void NAFE13388_Base::start( int ch )
{
	command( ch     );
	command( CMD_SS );
}

void NAFE13388_Base::command( uint16_t com )
{
	write_r16( com );
}

void NAFE13388_Base::reg( Register16 r, uint16_t value )
{
	write_r16( static_cast<uint16_t>( r ), value );
}

void NAFE13388_Base::reg( Register24 r, uint32_t value )
{
	write_r24( static_cast<uint16_t>( r ), value );
}

uint16_t NAFE13388_Base::reg( Register16 r )
{
	return read_r16( static_cast<uint16_t>( r ) );
}

uint32_t NAFE13388_Base::reg( Register24 r )
{
	return read_r24( static_cast<uint16_t>( r ) );
}

uint32_t NAFE13388_Base::part_number( void )
{
	return (static_cast<uint32_t>( reg( PN2 ) ) << 16) | reg( PN1 );
}

uint8_t NAFE13388_Base::revision_number( void )
{
	return reg( PN0 ) & 0xF;
}

uint64_t NAFE13388_Base::serial_number( void )
{
	uint64_t	serial_number;

	serial_number	  = reg( SERIAL1 );
	serial_number	<<=  24;
	return serial_number | reg( SERIAL0 );
}
			
float NAFE13388_Base::temperature( void )
{
	return reg( DIE_TEMP ) / 64.0;
}

void NAFE13388_Base::gain_offset_coeff( const ref_points &ref )
{
	constexpr double	pga1x_voltage		= 5.0;
	constexpr int		adc_resolution		= 24;
	constexpr double	pga_gain_setting	= 0.2;

	constexpr double	fullscale_voltage	= pga1x_voltage / pga_gain_setting;

	double	fullscale_data		= pow( 2, (adc_resolution - 1) );
	double	ref_data_span		= ref.high.data		- ref.low.data;
	double	ref_voltage_span	= ref.high.voltage	- ref.low.voltage;
	
	double	dv_slope			= ref_data_span / ref_voltage_span;
	double	custom_gain			= dv_slope * (fullscale_voltage / fullscale_data);
	double	custom_offset		= (dv_slope * ref.low.voltage - ref.low.data) / custom_gain;
	
	int32_t	gain_coeff_cal		= reg( GAIN_COEFF0   + ref.cal_index );
	int32_t	offsset_coeff_cal	= reg( OFFSET_COEFF0 + ref.cal_index );
	int32_t	gain_coeff_new		= round( gain_coeff_cal * custom_gain );
	int32_t	offset_coeff_new	= round( custom_offset - offsset_coeff_cal );

#if 0
	printf( "ref_point_high = %8ld @%6.3lf\r\n", ref.high.data, ref.high.voltage );
	printf( "ref_point_low  = %8ld @%6.3lf\r\n", ref.low.data,  ref.low.voltage  );
	printf( "gain_coeff_new   = %8ld\r\n", gain_coeff_new   );
	printf( "offset_coeff_new = %8ld\r\n", offset_coeff_new );
#endif
	
	reg( GAIN_COEFF0   + ref.coeff_index, gain_coeff_new   );
	reg( OFFSET_COEFF0 + ref.coeff_index, offset_coeff_new );
}

void NAFE13388_Base::recalibrate( int pga_gain_index, int channel_selection, int input_select, double reference_source_voltage, bool use_positive_side )
{
	constexpr	auto	low_gain_index	= 4;
	auto				channel_in_use	= false;
	ch_setting_t		tmp_ch_config;
	
	if ( reg( CH_CONFIG4 ) & (0x1 << channel_selection) )
	{
		channel_in_use	= true;
		
		command( channel_selection );

		for ( auto i = 0; i < 4; i++ )
			tmp_ch_config[ i ]	= reg( CH_CONFIG0 + i );
	}
	
	if ( !input_select )
	{
		if ( pga_gain_index <= low_gain_index )
		{
			input_select	= 0x5;	//	REFH for low gain
			reference_source_voltage	= 2.30;
		}
		else
		{
			input_select	= 0x6;	//	REFL for high gain
			reference_source_voltage	= 0.20;
		}
	}
	
	const uint16_t		REF_GND		= 0x0010  | (pga_gain_index << 5);
	const uint16_t		REF_V		= (input_select << (use_positive_side ? 12 : 8)) | REF_GND;
	const uint16_t		ch_config1	= (pga_gain_index << 12) | 0x00E4;
	constexpr uint16_t	ch_config2	= 0x8400;
	constexpr uint16_t	ch_config3	= 0x0000;

	const ch_setting_t	refh	= { REF_V,   ch_config1, ch_config2, ch_config3 };
	const ch_setting_t	refg	= { REF_GND, ch_config1, ch_config2, ch_config3 };

	logical_ch_config( channel_selection, refh );	
	raw_t	data_REF	= read<raw_t>( channel_selection );

	logical_ch_config( channel_selection, refg );
	raw_t	data_GND	= read<raw_t>( channel_selection );

	constexpr double	pga_gain[]	= { 0.2, 0.4, 0.8, 1, 2, 4, 8, 16 };

	const double	fullscale_voltage	= 5.00 / pga_gain[ pga_gain_index ];
	const double	calibrated_gain		= pow( 2, 23 ) * (reference_source_voltage / fullscale_voltage) / (double)(data_REF - data_GND);

#if 0
	printf( "data_REF = %8ld\r\n", data_REF );
	printf( "data_GND = %8ld\r\n", data_GND  );
	printf( "gain adjustment = %8lf (%lfdB)\r\n", calibrated_gain, 20 * log10( calibrated_gain ) );
#endif
	
	const double	current_gain_coeff_value	= (double)reg( GAIN_COEFF0 + pga_gain_index );
	const uint32_t	current_offset_coeff_value	= reg( OFFSET_COEFF0 + pga_gain_index );

	reg( GAIN_COEFF0   + pga_gain_index, (uint32_t)(current_gain_coeff_value * calibrated_gain) );
	reg( OFFSET_COEFF0 + pga_gain_index, current_offset_coeff_value + data_GND );

	if ( channel_in_use )
		logical_ch_config( channel_selection, tmp_ch_config );
	else
		logical_ch_disable( channel_selection );
}



/* NAFE13388 class ******************************************/

NAFE13388::NAFE13388( SPI& spi, int nINT, int DRDY, int SYN, int nRESET ) 
	: NAFE13388_Base( spi, nINT, DRDY, SYN, nRESET )
{
}

NAFE13388::~NAFE13388()
{
}

/* NAFE13388_UIM class ******************************************/

NAFE13388_UIM::NAFE13388_UIM( SPI& spi, int nINT, int DRDY, int SYN, int nRESET ) 
	: NAFE13388_Base( spi, nINT, DRDY, SYN, nRESET )
{
}

NAFE13388_UIM::~NAFE13388_UIM()
{
}

//double	NAFE13388::coeff_uV[ 16 ];
