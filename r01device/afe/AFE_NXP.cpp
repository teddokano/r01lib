/** NXP Analog Front End class library for MCX
 *
 *  @author  Tedd OKANO
 *
 *  Copyright: 2023 - 2025 Tedd OKANO
 *  Released under the MIT license
 */

#include	"AFE_NXP.h"
#include	"r01lib.h"

using enum	NAFE13388_Base::Register16;
using enum	NAFE13388_Base::Register24;

/* NAFE13388_Base class ******************************************/

NAFE13388_Base::NAFE13388_Base( SPI& spi, int nINT, int DRDY, int SYN, int nRESET ) : 
	SPI_for_AFE( spi ), enabled_channels( 0 ), pin_nINT( nINT ), pin_DRDY( DRDY ), pin_SYN( SYN, 1 ), pin_nRESET( nRESET, 1 )
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

void NAFE13388_Base::logical_ch_config( int ch, uint16_t cc0, uint16_t cc1, uint16_t cc2, uint16_t cc3 )
{	
	constexpr double	pga_gain[]	= { 0.2, 0.4, 0.8, 1, 2, 4, 8, 16 };
	
	command( ch );
	
	reg( CH_CONFIG0, cc0 );
	reg( CH_CONFIG1, cc1 );
	reg( CH_CONFIG2, cc2 );
	reg( CH_CONFIG3, cc3 );
	
	const uint16_t	setbit	= 0x1 << ch;
	const uint16_t	bits	= bit_op( CH_CONFIG4, ~setbit, setbit );

	enabled_channels	= 0;
	
	for ( int i = 0; i < 16; i++ ) {
		if ( bits & (0x1 << i) )
			enabled_channels++;
	}
	
	if ( cc0 & 0x0010 )
		coeff_uV[ ch ]	= ((10.0 / (double)(1L << 24)) / pga_gain[ (cc0 >> 5) & 0x7 ]) * 1e6;
	else
		coeff_uV[ ch ]	= (4.0 / (double)(1L << 24)) * 1e6;
}

void NAFE13388_Base::logical_ch_config( int ch, const uint16_t (&cc)[ 4 ] )
{	
	logical_ch_config( ch, cc[ 0 ], cc[ 1 ], cc[ 2 ], cc[ 3 ] );
}

template<> 
int32_t NAFE13388_Base::read( int ch, float delay )
{
	start_and_delay( ch, delay );
	return reg( CH_DATA0 + ch );
};

template<>
double NAFE13388_Base::read( int ch, float delay )
{
	return read<int32_t>( ch, delay ) * coeff_uV[ ch ];
};

void NAFE13388_Base::start( int ch )
{
	command( ch     );
	command( CMD_SS );
}

void NAFE13388_Base::start_and_delay( int ch, float delay )
{
	if ( delay >= 0.0 )
	{
		start( ch );
		wait( delay );
	}
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

/* NAFE13388 class ******************************************/

NAFE13388::NAFE13388( SPI& spi, int nINT, int DRDY, int SYN, int nRESET ) : NAFE13388_Base( spi, nINT, DRDY, SYN, nRESET )
{
}

NAFE13388::~NAFE13388()
{
}

/* NAFE13388_UIM class ******************************************/

NAFE13388_UIM::NAFE13388_UIM( SPI& spi, int nINT, int DRDY, int SYN, int nRESET ) : NAFE13388_Base( spi, nINT, DRDY, SYN, nRESET )
{
}

NAFE13388_UIM::~NAFE13388_UIM()
{
}

//double	NAFE13388::coeff_uV[ 16 ];
