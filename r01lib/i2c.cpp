/*
 *  @author Tedd OKANO
 *
 *  Released under the MIT license License
 */

extern "C" {
#include <stdio.h>
#include <string.h>
#include "pin_mux.h"
#include "clock_config.h"
#include "board.h"
#include "fsl_debug_console.h"
#include "fsl_lpi2c.h"
#include "fsl_port.h"
}

#include	"i2c.h"
#include	"io.h"
#include	"mcu.h"

#ifdef	CPU_MCXN947VDF
	#define EXAMPLE_I2C_MASTER_BASE			(LPI2C2_BASE)
	#define LPI2C_MASTER_CLOCK_FREQUENCY 	CLOCK_GetLPFlexCommClkFreq(2u)
	#define EXAMPLE_I2C_MASTER				((LPI2C_Type *)EXAMPLE_I2C_MASTER_BASE)
#elif	CPU_MCXN236VDF
	#define EXAMPLE_I2C_MASTER_BASE			(LPI2C2_BASE)
	#define LPI2C_MASTER_CLOCK_FREQUENCY	CLOCK_GetLPFlexCommClkFreq(2u)
	#define EXAMPLE_I2C_MASTER				((LPI2C_Type *)EXAMPLE_I2C_MASTER_BASE)
#elif	CPU_MCXA153VLH
	#define EXAMPLE_I2C_MASTER_BASE			LPI2C0
	#define LPI2C_MASTER_CLOCK_FREQUENCY	CLOCK_GetLpi2cClkFreq()
	#define EXAMPLE_I2C_MASTER				((LPI2C_Type *)EXAMPLE_I2C_MASTER_BASE)
#else
	#error Not supported CPU
#endif

I2C::I2C( int sda, int scl, bool no_hw ) : Obj( true )
{
	if ( no_hw )
		return;
	
#ifdef	CPU_MCXN947VDF
	if ( (sda == I2C_SDA) && (scl == I2C_SCL) )
		;
	else
		panic( "FRDM-MCXN947 only support I2C_SDA(D18)/I2C_SCL(D19) pins for I2C" );
	
	constexpr int	mux_setting	= 2;
#elif	CPU_MCXN236VDF
	if ( (sda == A4) && (scl == A5) )
		;
	else if ( (sda == MB_SDA) && (scl == MB_SCL) )
		;
	else
		panic( "FRDM-MCXN947 only support I2C_SDA(D18)/I2C_SCL(D19) pins for I2C" );
	
	constexpr int	mux_setting	= 2;
#elif	CPU_MCXA153VLH
	if ( (sda == I3C_SDA) && (scl == I3C_SCL) )
		;
	else if ( (sda == I2C_SDA) && (scl == I2C_SCL) )
		;
	else if ( (sda == MB_SDA) && (scl == MB_SCL) )
		;
	else if ( (sda == MB_MOSI) && (scl == MB_SCK) )
		;
	else
		panic( "FRDM-MCXA153 supports I3C_SDA/I3C_SCL, I2C_SDA(D18)/I2C_SCL(D19), MB_SDA/MB_SCL or MB_MOSI/MB_SCK pins for I2C" );

	constexpr int	mux_setting	= kPORT_MuxAlt3;
	RESET_ReleasePeripheralReset( kLPI2C0_RST_SHIFT_RSTn );
#else
#endif

	LPI2C_MasterGetDefaultConfig( &masterConfig );
	LPI2C_MasterInit( EXAMPLE_I2C_MASTER, &masterConfig, LPI2C_MASTER_CLOCK_FREQUENCY );
	
//	frequency( I2C_FREQ );
	
	DigitalInOut	_scl( scl );
	DigitalInOut	_sda( sda );
	
	_scl.pin_mux( mux_setting );
	_sda.pin_mux( mux_setting );
}

I2C::~I2C()
{
	LPI2C_MasterDeinit( EXAMPLE_I2C_MASTER );
}

void I2C::frequency( uint32_t frequency )
{
	LPI2C_MasterSetBaudRate( EXAMPLE_I2C_MASTER, LPI2C_MASTER_CLOCK_FREQUENCY, frequency );
}

status_t I2C::write( uint8_t address, const uint8_t *dp, int length, bool stop )
{
	status_t	r = kStatus_Fail;
	size_t		txCount	= 0xFFU;

	if ( (r = LPI2C_MasterStart( EXAMPLE_I2C_MASTER, address, kLPI2C_Write )) )
		return r;

	LPI2C_MasterGetFifoCounts(EXAMPLE_I2C_MASTER, NULL, &txCount);
	while (txCount)
	{
		LPI2C_MasterGetFifoCounts(EXAMPLE_I2C_MASTER, NULL, &txCount);
	}

	if ( LPI2C_MasterGetStatusFlags( EXAMPLE_I2C_MASTER ) & kLPI2C_MasterNackDetectFlag )
		return kStatus_LPI2C_Nak;

	if ( (r	= LPI2C_MasterSend( EXAMPLE_I2C_MASTER, (uint8_t *)dp, length )) )
	{
		if ( r == kStatus_LPI2C_Nak )
			LPI2C_MasterStop( EXAMPLE_I2C_MASTER );

		return r;
	}

	if ( stop )
		return LPI2C_MasterStop( EXAMPLE_I2C_MASTER );

	return kStatus_Success;
}

status_t I2C::read( uint8_t address, uint8_t *dp, int length, bool stop )
{
	status_t	r = kStatus_Fail;
	size_t		txCount	= 0xFFU;

	if ( (r = LPI2C_MasterStart( EXAMPLE_I2C_MASTER, address, kLPI2C_Read )) )
		return r;

	do {
		LPI2C_MasterGetFifoCounts( EXAMPLE_I2C_MASTER, NULL, &txCount );
	} while ( txCount );

	if ( LPI2C_MasterGetStatusFlags( EXAMPLE_I2C_MASTER ) & kLPI2C_MasterNackDetectFlag )
		return kStatus_LPI2C_Nak;

	if ( (r	= LPI2C_MasterReceive( EXAMPLE_I2C_MASTER, dp, length )) )
		return r;

	if ( stop )
		return LPI2C_MasterStop( EXAMPLE_I2C_MASTER );

	return kStatus_Success;
}

status_t I2C::reg_write( uint8_t targ, uint8_t reg, const uint8_t *dp, int length )
{
	uint8_t	bp[ REG_RW_BUFFER_SIZE ];
	
	bp[ 0 ]	= reg;
	memcpy( (uint8_t *)bp + 1, (uint8_t *)dp, length );

	last_status	= write( targ, bp, length + 1 );
	
	return last_status;
}

status_t I2C::reg_write( uint8_t targ, uint8_t reg, uint8_t data )
{
	return write( targ, &data, sizeof( data ) );
}

status_t I2C::reg_read( uint8_t targ, uint8_t reg, uint8_t *dp, int length )
{
	last_status	= write( targ, &reg, sizeof( reg ), NO_STOP );
	
	if ( kStatus_Success != last_status )
		return last_status;
	
	return read( targ, dp, length );
}

uint8_t I2C::reg_read( uint8_t targ, uint8_t reg )
{
	last_status	= write( targ, reg, NO_STOP );
	return read( targ );
}

status_t I2C::write( uint8_t targ, uint8_t data, bool stop )
{
	return write( targ, &data, sizeof( data ), stop );
}

uint8_t I2C::read( uint8_t targ, bool stop )
{
	uint8_t	data;

	last_status	= read( targ, &data, sizeof( data ), stop );

	return data;
}

status_t I2C::ccc_set( uint8_t ccc, uint8_t addr, uint8_t data )
{
	return kStatus_Success;
}

status_t I2C::ccc_get( uint8_t ccc, uint8_t addr, uint8_t *dp, uint8_t length )
{
	memset( dp, 0, length );
	return kStatus_Success;
}
