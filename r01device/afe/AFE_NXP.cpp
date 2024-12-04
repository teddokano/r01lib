/** NXP Analog Front End class library for MCX
 *
 *  @author  Tedd OKANO
 *
 *  Copyright: 2023 - 2024 Tedd OKANO
 *  Released under the MIT license
 */

#include	"AFE_NXP.h"
#include	"r01lib.h"

/* NAFE13388_Base class ******************************************/

NAFE13388_Base::NAFE13388_Base( SPI& spi ) : SPI_for_AFE( spi ), enabled_channels( 0 )
{
}

NAFE13388_Base::~NAFE13388_Base()
{
}

void NAFE13388_Base::begin( void )
{
	reset();
	boot();	
}

void NAFE13388_Base::boot( void )
{
	write_r16( 0x0010 ); 
	write_r16( 0x002A, 0x0000 );
	write_r16( 0x002B, 0x0000 );
	write_r16( 0x002C, 0x0000 );
	write_r16( 0x002F, 0x0000 );
	write_r16( 0x0029, 0x0000 );
	wait( 0.001 );
	write_r16( 0x0030, 0x0010 );
	wait( 0.001 );
}

void NAFE13388_Base::reset( void )
{
	write_r16( 0x0014 ); 
	wait( 0.001 );
}

void NAFE13388_Base::board_init( int _pin_nINT, int _pin_DRDY, int _pin_SYN, int _pin_nRESET )
{
	DigitalIn	ADC_nINT(  _pin_nINT );
	DigitalIn	ADC_nDRDY( _pin_DRDY );

	DigitalOut	ADC_SYN(    _pin_SYN,    1 );
	DigitalOut	ADC_nRESET( _pin_nRESET, 1 );
}

void NAFE13388_Base::logical_ch_config( int ch, uint16_t cc0, uint16_t cc1, uint16_t cc2, uint16_t cc3 )
{	
	constexpr double	pga_gain[]	= { 0.2, 0.4, 0.8, 1, 2, 4, 8, 16 };
	
	write_r16( ch );
	
	write_r16( 0x0020, cc0 );
	write_r16( 0x0021, cc1 );
	write_r16( 0x0022, cc2 );
	write_r16( 0x0023, cc3 );
	
	uint16_t	mask	= 1;
	uint16_t	bits	= read_r16( 0x0024 ) | (mask << ch);
	enabled_channels	= 0;
	
	for ( int i = 0; i < 16; i++ ) {
		if ( bits & (mask << i) )
			enabled_channels++;
	}
	
	write_r16( 0x0024, bits );
	
	if ( cc0 & 0x0010 )
		coeff_uV[ ch ]	= ((10.0 / (double)(1L << 24)) / pga_gain[ (cc0 >> 5) & 0x7 ]) * 1e6;
	else
		coeff_uV[ ch ]	= (4.0 / (double)(1L << 24)) * 1e6;
}

template<> 
int32_t NAFE13388_Base::read( int ch, float delay )
{
	start_and_delay( ch, delay );
	return read_r24( 0x2040 + ch );
};

template<>
double NAFE13388_Base::read( int ch, float delay )
{
	return read<int32_t>( ch, delay ) * coeff_uV[ ch ];
};

void NAFE13388_Base::start( int ch )
{
	write_r16( ch );
	write_r16( 0x2000 );
}

void NAFE13388_Base::start_and_delay( int ch, float delay )
{
	if ( delay >= 0.0 )
	{
		start( ch );
		wait( delay );
	}
}

/* NAFE13388 class ******************************************/

NAFE13388::NAFE13388( SPI& spi ) : NAFE13388_Base( spi )
{
	board_init( D2, D3, D5, D6 );
}

NAFE13388::~NAFE13388()
{
}

/* NAFE13388_UIM class ******************************************/

NAFE13388_UIM::NAFE13388_UIM( SPI& spi ) : NAFE13388_Base( spi )
{
	board_init( D3, D4, D6, D7 );
}

NAFE13388_UIM::~NAFE13388_UIM()
{
}

//double	NAFE13388::coeff_uV[ 16 ];
