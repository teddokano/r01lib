#include	"i3c.h"
#include	"P3T1755.h"

extern "C" {
#include "fsl_debug_console.h"
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmisleading-indentation"

I3C_Device::I3C_Device( I2C &i2c, uint8_t address ) : _i2c( i2c ), _addr( address )
{	
}

I3C_Device::~I3C_Device(){}

void I3C_Device::address_overwrite( uint8_t address )
{
	_addr	= address;
}

void I3C_Device::ccc_set( CCC ccc, uint8_t data )
{
	_i2c.ccc_set( ccc, _addr, data );
}

uint8_t* I3C_Device::ccc_get( CCC ccc, uint8_t *dp, uint8_t length )
{
	_i2c.ccc_get( ccc, _addr, dp, length );
	
	return dp;
}




P3T1755::P3T1755( I2C &i2c, uint8_t address ) : I3C_Device( i2c, address ) {}
P3T1755::~P3T1755(){}

float P3T1755::temp( void )
{
	return short2celsius( read( Temp ) );
}

void P3T1755::conf( uint8_t config )
{
	_i2c.reg_write( _addr, Conf, &config, sizeof( config ) );
}

uint8_t P3T1755::conf( void )
{
	uint8_t	c;
	_i2c.reg_read( _addr, P3T1755::Conf, &c, sizeof( c ) );
	
	return c;
}

void P3T1755::high( float celsius )
{
	write( T_HIGH, celsius );
}

float P3T1755::high( void )
{
	return short2celsius( read( T_HIGH ) );
}

void P3T1755::low( float celsius )
{
	write( T_LOW, celsius );
}

float P3T1755::low( void )
{
	return short2celsius( read( T_LOW ) );
}

void P3T1755::write( uint8_t reg, float v )
{
	uint16_t	tmp	= swap_bytes( celsius2short( v ) );
	_i2c.reg_write( _addr, reg, (uint8_t *)&tmp, sizeof( tmp ) );
}

int16_t P3T1755::read( uint8_t reg )
{
	uint16_t	tmp;
	_i2c.reg_read( _addr, reg, (uint8_t *)&tmp, sizeof( tmp ) );
	
	return tmp;
}

void P3T1755::info( void )
{
	uint8_t		pid[ I3C::PID_LENGTH ];
	uint8_t		bcr, dcr;

	float	t	= temp();
	uint8_t	c	= conf();
	float	h	= high();
	float	l	= low();
	
	ccc_get( CCC::DIRECT_GETPID, pid, sizeof( pid ) );
	ccc_get( CCC::DIRECT_GETBCR, &bcr, 1 );
	ccc_get( CCC::DIRECT_GETDCR, &dcr, 1 );

	PRINTF( "\r\nRegister dump - I3C target address:7’h%02X (0x%02X)\r\n", P3T1755_ADDR_I3C, P3T1755_ADDR_I3C << 1 );
	PRINTF( "  - Temp   (0x0): 0x%04X (%8.4f˚C)\r\n", (uint16_t)P3T1755::celsius2short( t ), t );
	PRINTF( "  - Conf   (0x1): 0x  %02X\r\n", c );
	PRINTF( "  - T_LOW  (0x2): 0x%04X (%8.4f˚C)\r\n", (uint16_t)P3T1755::celsius2short( l ), l );
	PRINTF( "  - T_HIGH (0x3): 0x%04X (%8.4f˚C)\r\n", (uint16_t)P3T1755::celsius2short( h ), h );

	PRINTF( "  * PID    (CCC:Provisioned ID)                 : 0x" );
	for ( int i = 0; i < I3C::PID_LENGTH; i++ ) PRINTF( " %02X", pid[ i ] );	PRINTF( "\r\n" );
	PRINTF( "  * BCR    (CCC:Bus Characteristics Register)   : 0x%02X\r\n", bcr );
	PRINTF( "  * DCR    (CCC:Device Characteristics Register): 0x%02X (= %s)\r\n", dcr, (0x63 == dcr) ? "Temperature sensor" : "Unknown" );

	PRINTF( "\r\n" );
}


float P3T1755::short2celsius( int16_t v )
{
	return (float)(swap_bytes( v )) / 256.0;
}

int16_t P3T1755::celsius2short( float v )
{
	return (uint16_t)(v * 256.0);
}

int16_t	P3T1755::swap_bytes( int16_t v )
{
#ifdef __BIG_ENDIAN__
	return v;
#else	
	return (v << 8) | ((uint16_t)v >> 8);
#endif
}

P3T1755::operator float()
{
	return temp();
}

#pragma GCC diagnostic pop
