/*
 * Copyright 2024 Tedd OKANO
 */

#ifndef R01LIB_P3T1755_H
#define R01LIB_P3T1755_H

#include	"i3c.h"

#define P3T1755_ADDR_I2C			0x48U
#define P3T1755_ADDR_I3C			0x08U
#define P3T1755_CONFIG_VALUE		0x2A

class I3C_Device
{
public:
	I3C_Device( I2C &i2c, uint8_t address );
	~I3C_Device();
	
	void				address_overwrite( uint8_t address );

	virtual void		ccc_set( CCC ccc, uint8_t data );
	virtual uint8_t*	ccc_get( CCC ccc, uint8_t *dp, uint8_t length );

protected:
	I2C&	_i2c;
	uint8_t	_addr;	
};

class P3T1755 : public I3C_Device
{
public:
	enum	registers {
		Temp,
		Conf,
		T_LOW,
		T_HIGH,
	};

	P3T1755( I2C &i2c, uint8_t address = P3T1755_ADDR_I2C );
	~P3T1755();
	
	float		temp( void );
	
	void		conf( uint8_t conf );
	uint8_t		conf( void );
	
	void		high( float temp );
	float		high( void );
	
	void		low(  float temp );
	float		low(  void );

	void 		write( uint8_t reg, float v );
	int16_t 	read( uint8_t reg );

	void		info( void );
	
	static float	short2celsius( int16_t v );
	static int16_t	celsius2short( float v );
	static int16_t	swap_bytes( int16_t v );

	/** A short hand for reading pins
	 */
	operator	float();

private:
};

#endif // R01LIB_P3T1755_H
