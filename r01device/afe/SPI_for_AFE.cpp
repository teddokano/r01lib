/** NXP Analog Front End class library for MCX
 *
 *  @author  Tedd OKANO
 *
 *  Copyright: 2023 - 2024 Tedd OKANO
 *  Released under the MIT license
 */

#include "AFE_NXP.h"

#define	READ_BUFFER_SIZE	10

SPI_for_AFE::SPI_for_AFE( SPI& spi ) : _spi( spi )
{
}

SPI_for_AFE::~SPI_for_AFE()
{
}

void SPI_for_AFE::txrx( uint8_t *data, int size )
{
	uint8_t	r_data[ READ_BUFFER_SIZE ];
	
	_spi.write( data, r_data, size );
	memcpy( data, r_data, size );
}

void SPI_for_AFE::write_r16( uint16_t reg )
{
	reg	<<= 1;

	uint8_t	v[]	= { (uint8_t)(reg >> 8), (uint8_t)(reg & 0xFF) };
	txrx( v, sizeof( v ) );
}

void SPI_for_AFE::write_r16( uint16_t reg, uint16_t val )
{
	reg	<<= 1;

	uint8_t	v[]	= { (uint8_t)(reg >> 8), (uint8_t)(reg & 0xFF), (uint8_t)(val >> 8), (uint8_t)val };
	txrx( v, sizeof( v ) );
}

uint16_t SPI_for_AFE::read_r16( uint16_t reg )
{
	reg	<<= 1;
	reg	 |= 0x4000;

	uint8_t	v[ 4 ]	= { (uint8_t)(reg >> 8), (uint8_t)(reg & 0xFF), 0xFF, 0xFF };
	txrx( v, sizeof( v ) );
	
	return (uint16_t)(v[ 2 ]) << 8 | v[ 3 ];
}

void SPI_for_AFE::write_r24( uint16_t reg, uint32_t val )
{
	reg	<<= 1;

	uint8_t	v[]	= { (uint8_t)(reg >> 8), (uint8_t)(reg & 0xFF), (uint8_t)(val >> 16), (uint8_t)(val >> 8), (uint8_t)val };
	txrx( v, sizeof( v ) );
}

int32_t SPI_for_AFE::read_r24( uint16_t reg )
{
	reg	<<= 1;
	reg	 |= 0x4000;

	uint8_t	v[]	= { (uint8_t)(reg >> 8), (uint8_t)(reg & 0xFF), 0xFF, 0xFF, 0xFF };
	txrx( v, sizeof( v ) );
	
	int32_t	r0	= v[ 2 ];
	int32_t	r1	= v[ 3 ];
	int32_t	r2	= v[ 4 ];
	int32_t	r	= ( (r0 << 24) | (r1 << 16) | (r2 << 8) );

	return r >> 8;
}
