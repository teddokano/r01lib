/*
 * Copyright 2024 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * This code based on a sample projects: 
 *    "frdmmcxa153_i3c_master_read_sensor_p3t1755" from SDK_2.14.2_FRDM-MCXA153 and 
 *    "lpcxpresso860max_i3c_master_read_sensor_icm42688p" from SDK_2.15.000_LPCXpresso860MAX
 */

/** I3C class
 *	
 *  @class I3C
 *
 *	A class for demonstrating I3C bus
 */

#ifndef R01LIB_I3C_H
#define R01LIB_I3C_H

#include	"i2c.h"
#include	"fsl_i3c.h"

#define	CUSTOM_REGISTAR_XFER

/** constants: CCC  */
enum CCC
{
	BROADCAST_ENEC		= 0x00,
	BROADCAST_RSTDAA	= 0x06,
	BROADCAST_ENTDAA	= 0x07,
	DIRECT_ENEC			= 0x80,
	DIRECT_DICEC		= 0x81,
	DIRECT_SETDASA		= 0x87,
	DIRECT_SETNEWDA		= 0x88,
	DIRECT_GETPID		= 0x8D,
	DIRECT_GETBCR		= 0x8E,
	DIRECT_GETDCR		= 0x8F,
	DIRECT_GETSTATUS	= 0x90,
	DIRECT_RSTACT		= 0x90
};

typedef void (*i3c_func_ptr)(void); 

class I3C : public I2C
{
public:
	/** constants for mode setting  */
	enum MODE
	{
		I3C_MODE	= kI3C_TypeI3CSdr,
		I2C_MODE	= kI3C_TypeI2C,
		I3CDDR_MODE	= kI3C_TypeI3CDdr
	};
	
	/** constants for SCL frequency settings  */
	enum FREQ
	{
		OD_FREQ					= 4000000UL,
		PP_FREQ					= 12500000UL,
		DEFAULT_FREQ_SETTING	= 0
	};
	
	/** constants for miscellaneous setting  */
	enum MISC
	{
		BROADCAST_ADDR	= 0x7E,
		PID_LENGTH		= 6
	};

	/** Create an I3C instance with specified pins
	 *
	 * @param sda pin number to connect SDA
	 * @param scl pin number to connect SCL
	 * @param i2c_freq (option) define default scl frequency while I2C operation
	 * @param i3c_od_freq (option) define default scl frequency while I3C open-drain operation
	 * @param i3c_pp_freq (option) define default scl frequency while I3C push-pull operation
	 */
	I3C( int sda, int scl, uint32_t i2c_freq = I2C::FREQ, uint32_t i3c_od_freq = OD_FREQ, uint32_t i3c_pp_freq = PP_FREQ );

	/** Destractor to freeing I3C resource
	 */
	~I3C();
	
	/** Frequency settings
	 * 
	 * @param i2c_freq (option) define default scl frequency while I2C operation
	 * @param i3c_od_freq (option) define default scl frequency while I3C open-drain operation
	 * @param i3c_pp_freq (option) define default scl frequency while I3C push-pull operation
	 * 
	 *  @note use zero or I3C_DEFAULT_FREQ to set default frequency
	 */
	void		frequency( uint32_t i2c_freq, uint32_t i3c_od_freq, uint32_t i3c_pp_freq );

	/** All frequency settings reverted to default
	 */
	void		frequency( void );

	/** mode setting
	 *	I3C bus is configured to I3C-SDR, I3C-DDR or I2C
	 *  
	 * @param mode kI3C_TypeI3CSdr, kI3C_TypeI2C or kI3C_TypeI3CDdr
	 */
	void 		mode( MODE mode );

	/** write transaction
	 *
	 * @param targ target address
	 * @param dp data to write
	 * @param length data length
	 * @param stop (option) generate STOP condition: "false" to make repeated-start in next transaction
	 * @return status_t
	 */
	status_t	write( uint8_t targ, const uint8_t *dp, int length, bool stop = STOP );	

	/** read transaction
	 *
	 * @param targ target address
	 * @param dp data to write
	 * @param length data length
	 * @param stop (option) generate STOP condition: "false" to make repeated-start in next transaction
	 * @return status_t
	 */
	status_t	read( uint8_t targ, uint8_t *dp, int length, bool stop = STOP );
	
#ifdef	CUSTOM_REGISTAR_XFER
	/** Register write (multiple byte data)
	 *	provideds interface for register write
	 *	
	 * @param targ target address
	 * @param reg register address
	 * @param dp data to write
	 * @param length data length
	 * @return status_t
	 */
	status_t	reg_write( uint8_t targ, uint8_t reg, const uint8_t *dp, int length, bool stop = STOP );

	/** Register read (multiple byte data)
	 *	provideds interface for register read
	 *	
	 * @param targ target address
	 * @param reg register address
	 * @param dp data to write
	 * @param length data length
	 * @return status_t
	 */
	status_t	reg_read( uint8_t targ, uint8_t reg, uint8_t *dp, int length, bool stop = STOP );
#endif	// CUSTOM_REGISTAR_XFER
	
	/** check IBI status
	 *  
	 * @return target address of IBI initiated device or zero if no event happened
	 */
	uint8_t		check_IBI( void );
	
	/** set IBI callback function
	 *  
	 * @return target address of IBI initiated device or zero if no event happened
	 */
	void		set_IBI_callback( i3c_func_ptr fp );

	/** CCC broadcast
	 *  
	 * @param ccc CCC command
	 * @param dp data to send
	 * @param length data length
	 * @return status_t
	 */
	status_t	ccc_broadcast( uint8_t ccc, const uint8_t *dp, uint8_t length, bool first_time = false );

	/** CCC set
	 *  
	 * @param ccc CCC command
	 * @param data single byte data
	 * @return status_t
	 */
	status_t	ccc_set( uint8_t ccc, uint8_t addr, uint8_t data );

	/** CCC get
	 *  
	 * @param ccc CCC command
	 * @param dp data to send
	 * @param length data length
	 * @return status_t
	 */
	status_t	ccc_get( uint8_t ccc, uint8_t addr, uint8_t *dp, uint8_t length );

	/** perform DAA procedure
	 *  
	 * @param address_list new address list to be assigned
	 * @param list_length address list length
	 * @param device_list pointer to i3c_device_info_t array which is dealeared as static array in I3C driver
	 * @return int for number of devices which has newly assigned addresses (max 10)
	 */
	int			DAA( const uint8_t *address_list, uint8_t list_length, i3c_device_info_t** device_list );
	
	/** master_ibi_callback
	 *  interface function for SDK
	 */	
	static void		master_ibi_callback( I3C_Type *base, i3c_master_handle_t *handle, i3c_ibi_type_t ibiType, i3c_ibi_state_t ibiState );

	/** master_ibi_callback
	 *  interface function for SDK
	 */	
	static void		master_callback( I3C_Type *base, i3c_master_handle_t *handle, status_t status, void *userData );

private:
	status_t	xfer( i3c_direction_t dir, i3c_bus_type_t type, uint8_t targ, uint8_t *dp, int length, bool stop = STOP );

#ifdef	CUSTOM_REGISTAR_XFER
	status_t 	reg_xfer( i3c_direction_t dir, i3c_bus_type_t type, uint8_t targ, uint8_t reg, uint8_t reg_length, uint8_t *dp, int length, bool stop = STOP );
#endif	// CUSTOM_REGISTAR_XFER

	i3c_bus_type_t								bus_type;
	static const i3c_master_transfer_callback_t	masterCallback;
	i3c_master_config_t							masterConfig;
	bool										first_broadcast;
};

#endif // R01LIB_I3C_H
