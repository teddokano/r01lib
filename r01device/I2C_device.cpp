#include "I2C_device.h"

Serial_device::Serial_device() {}
Serial_device::~Serial_device() {}

void 		Serial_device::write_r16( uint8_t reg, uint16_t val ) {}
uint16_t	Serial_device::read_r16( uint8_t reg ) { return 0; }
void		Serial_device::bit_op16( uint8_t reg, uint16_t mask, uint16_t value ) {}

I2C_device::I2C_device( I2C& interface, uint8_t i2c_address, bool repeated_start_enable ) : i2c( interface ), i2c_addr( i2c_address ), rs_dis( !repeated_start_enable )
{
}

I2C_device::~I2C_device()
{
}

void I2C_device::repeated_start_enable( bool en )
{
	rs_dis	= !en;
}

void I2C_device::scan( I2C& target_i2c, uint8_t stop )
{
	bool  result[ 128 ];
	uint8_t	dummy	= 0;

	for ( uint8_t i = 0; i < stop; i++ ) {
		result[i] = !target_i2c.write( i, &dummy, 0 );
	}

	for ( uint8_t i = stop; i < 128; i++ ) {
		result[i] = false;
	}

	PRINTF( "\r\nI2C scan result\r\n   " );
	for ( uint8_t x = 0; x < 16; x++ )
		PRINTF( " %02X", x );
	
	for ( uint8_t i = 0; i < 128; i++ ) {
		if ( !( i % 16) )
			PRINTF( "\r\n%02Xx:", i / 16 );

		if ( result[ i ] )
			PRINTF( " %02X", i );
		else
			PRINTF( " --" );
	}
	PRINTF( "\r\n" );			
}

int I2C_device::tx( const uint8_t *data, uint16_t size, bool stop )
{
	return i2c.write( i2c_addr, data, size );
}

int I2C_device::rx( uint8_t *data, uint16_t size )
{
	return i2c.read( i2c_addr, data, size );
}

int I2C_device::reg_w( uint8_t reg_adr, const uint8_t *data, uint16_t size )
{
	uint8_t buffer[ size + 1 ];
	
	buffer[ 0 ]	= reg_adr;
	for ( uint16_t i = 0; i < size; i++)
		buffer[ i + 1 ]	= data[ i ];
	
	return tx( buffer, sizeof( buffer ) );
}

int I2C_device::reg_w( uint8_t reg_adr, uint8_t data )
{
	uint8_t buffer[ 2 ];
	
	buffer[ 0 ]	= reg_adr;
	buffer[ 1 ]	= data;
	
	return tx( buffer, sizeof( buffer ) );
}

int I2C_device::reg_r( uint8_t reg_adr, uint8_t *data, uint16_t size )
{
	tx( &reg_adr, 1, rs_dis );
	return rx( data, size );
}

uint8_t I2C_device::reg_r( uint8_t reg_adr )
{
	uint8_t	buffer	= 0;	//	assignning zero to suppress warning "-Wmaybe-uninitialized"
	
	tx( &reg_adr, 1, rs_dis );
	rx( &buffer, 1 );
	return buffer;
} 

void I2C_device::write_r8( uint8_t reg, uint8_t val )
{
	reg_w( reg, val );
}

void I2C_device::write_r16( uint8_t reg, uint16_t val )
{
	uint8_t	buff[ 2 ];
	
	buff[ 0 ]	= val >> 8;
	buff[ 1 ]	= val & 0xFF;
	
	reg_w( reg, buff, sizeof( buff ) );
}

uint8_t I2C_device::read_r8( uint8_t reg )
{
	return reg_r( reg );	
}

uint16_t I2C_device::read_r16( uint8_t reg )
{
	uint8_t	buff[ 2 ];

	reg_r( reg, buff, sizeof( buff ) );
	
	return (buff[ 0 ] << 8) | buff[ 1 ];
}

void I2C_device::bit_op8( uint8_t reg, uint8_t mask, uint8_t value )
{
	uint8_t	v	= read_r8( reg );

	v	&= mask;
	v	|= value;
	
	write_r8( reg, v );
}

void I2C_device::bit_op16( uint8_t reg, uint16_t mask, uint16_t value )
{
	uint16_t v	= read_r16( reg );

	v	&= mask;
	v	|= value;

	write_r16( reg, v );
}

uint8_t I2C_device::address( void )
{
	return i2c_addr;
}

void I2C_device::address_overwrite( uint8_t address )
{
	i2c_addr	= address;
}

void I2C_device::ccc_set( CCC ccc, uint8_t data )
{
	i2c.ccc_set( ccc, i2c_addr, data );
}

uint8_t* I2C_device::ccc_get( CCC ccc, uint8_t *dp, uint8_t length )
{
	i2c.ccc_get( ccc, i2c_addr, dp, length );
	
	return dp;
}

