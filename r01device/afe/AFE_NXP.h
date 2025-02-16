/** NXP Analog Front End class library for MCX
 *
 *  @class   NAFE13388
 *  @author  Tedd OKANO
 *
 *  Copyright: 2023 - 2025 Tedd OKANO
 *  Released under the MIT license
 *
 *  A simple class library for NXP Analog Front End: NAFE13388
 *
 *  Example:
 *  @code
 *  #include	"r01lib.h"
 *  #include	"afe/NAFE13388_UIM.h"
 *  
 *  SPI				spi( D11, D12, D13, D10 );	//	MOSI, MISO, SCLK, CS
 *  NAFE13388_UIM	afe( spi );
 *  
 *  int main( void )
 *  {
 *  	printf( "***** Hello, NAFE13388 UIM board! *****\r\n" );
 *  
 *  	spi.frequency( 1000 * 1000 );
 *  	spi.mode( 1 );
 *  
 *  	afe.begin();
 *  
 *  	afe.logical_ch_config( 0, 0x1070, 0x0084, 0x2900, 0x0000 );
 *  	afe.logical_ch_config( 1, 0x2070, 0x0084, 0x2900, 0x0000 );
 *  
 *  	while ( true )
 *  	{		
 *  		printf( "microvolt: %11.2f, %11.2f\r\n", afe.read<NAFE13388::microvolt_t>( 0, 0.01 ), afe.read<NAFE13388::microvolt_t>( 1, 0.01 ) );
 *  		printf( "raw:       %ld, %ld\r\n",       afe.read<NAFE13388::raw_t>( 0, 0.01 ),       afe.read<NAFE13388::raw_t>( 1, 0.01 )       );
 *  
 *  		wait( 0.05 );
 *  	}
 *  }
 *  @endcode
 */

#ifndef ARDUINO_AFE_DRIVER_H
#define ARDUINO_AFE_DRIVER_H

#include	<stdint.h>
#include	"r01lib.h"
#include	"SPI_for_AFE.h"

class NAFE13388_Base : public SPI_for_AFE
{
public:	
	
	/** ADC readout types */
	using raw_t			= int32_t;
	using microvolt_t	= double;
	constexpr static float immidiate_read	= -1.0;

	/** Constructor to create a NAFE13388 instance */
	NAFE13388_Base( SPI& spi, int nINT, int DRDY, int SYN, int nRESET );

	/** Destractor */
	virtual ~NAFE13388_Base();
	
	/** Begin the device operation
	 *
	 *	NAFE13388 initialization. It does following steps
	 *	(1) Set pins 2 and 3 are input for nINT and nDRDY
	 *	(2) Set pins 5 and 6 are output and fixed to HIGH for ADC_SYN and ADC_nRESET
	 *	(3) Call reset()
	 *	(4) Call boot()
	 */
	virtual void begin( void );

	/** Set system-level config registers */
	virtual void boot( void );

	/** Issue RESET command */
	virtual void reset( bool hardware_reset = false );
	
	/** Configure logical channel
	 *
	 * @param ch logical channel number (0 ~ 15)
	 * @param cc0	16bit value to be set CH_CONFIG0 register (0x20)
	 * @param cc1	16bit value to be set CH_CONFIG1 register (0x21)
	 * @param cc2	16bit value to be set CH_CONFIG2 register (0x22)
	 * @param cc3	16bit value to be set CH_CONFIG3 register (0x23)
	 */
	virtual void logical_ch_config( int ch, uint16_t cc0, uint16_t cc1, uint16_t cc2, uint16_t cc3 );

	/** Configure logical channel
	 *
	 * @param ch logical channel number (0 ~ 15)
	 * @param cc array for CH_CONFIG0, CH_CONFIG1, CH_CONFIG2 and CH_CONFIG3 values
	 */
	virtual void logical_ch_config( int ch, const uint16_t (&cc)[ 4 ] );

	/** Read ADC
	 *	Performs ADC read. 
	 *	If the delay is not given, just the ADC register is read.
	 *	If the delay is given, measurement is started in this method and read-out after delay.
	 *	The delay between start and read-out is specified in seconds. 
	 *	
	 *	This method need to be called with return type as 
	 *	    double value = read<NAFE13388::microvolt_t>( 0, 0.01 );
	 *	    int32_t value = read<NAFE13388::raw_t>( 0, 0.01 );
	 *	
	 * @param ch logical channel number (0 ~ 15)
	 * @param delay ADC result read-out delay after measurement start if given
	 * @return ADC readout value
	 */
	template<class T>
	T read( int ch, float delay = immidiate_read );

	/** Start ADC
	 *
	 * @param ch logical channel number (0 ~ 15)
	 */
	virtual void start( int ch );

	/** Number of enabled logical channels */
	int		enabled_channels;
	
	/** Coefficient to convert from ADC read value to micro-volt */
	double	coeff_uV[ 16 ];
	
private:
	void start_and_delay( int ch, float delay );
	
public:
	enum class Register16 : uint16_t {
		CH_CONFIG0				= 0x20,
		CH_CONFIG1				= 0x21,
		CH_CONFIG2				= 0x22,
		CH_CONFIG3				= 0x23,
		CH_CONFIG4				= 0x24,
		CRC_CONF_REGS			= 0x25,
		CRC_COEF_REGS			= 0x26,
		CRC_TRIM_REGS			= 0x27,
		GPI_DATA				= 0x29,
		GPIO_CONFIG0			= 0x2A,
		GPIO_CONFIG1			= 0x2B,
		GPIO_CONFIG2			= 0x2C,
		GPI_EDGE_POS			= 0x2D,
		GPI_EDGE_NEG			= 0x2E,
		GPO_DATA				= 0x2F,
		SYS_CONFIG0				= 0x30,
		SYS_STATUS0				= 0x31,
		GLOBAL_ALARM_ENABLE		= 0x32,
		GLOBAL_ALARM_INTERRUPT	= 0x33,
		DIE_TEMP				= 0x34,
		CH_STATUS0				= 0x35,
		CH_STATUS1				= 0x36,
		THRS_TEMP				= 0x37,
		PN2						= 0x7C,
		PN1						= 0x7D,
		PN0						= 0x7E,
		CRC_TRIM_INT			= 0x7F,
	};

	enum class Register24 : uint16_t {
		CH_DATA0		= 0x40,
		CH_DATA1		= 0x41,
		CH_DATA2		= 0x42,
		CH_DATA3		= 0x43,
		CH_DATA4		= 0x44,
		CH_DATA5		= 0x45,
		CH_DATA6		= 0x46,
		CH_DATA7		= 0x47,
		CH_DATA8		= 0x48,
		CH_DATA9		= 0x4A,
		CH_DATA10		= 0x4B,
		CH_DATA11		= 0x4C,
		CH_DATA13		= 0x4D,
		CH_DATA14		= 0x4E,
		CH_DATA15		= 0x4F,
		CH_CONFIG5_0	= 0x50,
		CH_CONFIG5_1	= 0x51,
		CH_CONFIG5_2	= 0x52,
		CH_CONFIG5_3	= 0x53,
		CH_CONFIG5_4	= 0x54,
		CH_CONFIG5_5	= 0x55,
		CH_CONFIG5_6	= 0x56,
		CH_CONFIG5_7	= 0x57,
		CH_CONFIG5_8	= 0x58,
		CH_CONFIG5_9	= 0x59,
		CH_CONFIG5_10	= 0x5A,
		CH_CONFIG5_11	= 0x5B,
		CH_CONFIG5_12	= 0x5C,
		CH_CONFIG5_13	= 0x5D,
		CH_CONFIG5_14	= 0x5E,
		CH_CONFIG5_15	= 0x5F,
		CH_CONFIG6_0	= 0x60,
		CH_CONFIG6_1	= 0x61,
		CH_CONFIG6_2	= 0x62,
		CH_CONFIG6_3	= 0x63,
		CH_CONFIG6_4	= 0x64,
		CH_CONFIG6_5	= 0x65,
		CH_CONFIG6_6	= 0x66,
		CH_CONFIG6_7	= 0x67,
		CH_CONFIG6_8	= 0x68,
		CH_CONFIG6_9	= 0x69,
		CH_CONFIG6_10	= 0x6A,
		CH_CONFIG6_11	= 0x6B,
		CH_CONFIG6_12	= 0x6C,
		CH_CONFIG6_13	= 0x6D,
		CH_CONFIG6_14	= 0x6E,
		CH_CONFIG6_15	= 0x6F,
		GAIN_COEFF0		= 0x80,
		GAIN_COEFF1		= 0x81,
		GAIN_COEFF2		= 0x82,
		GAIN_COEFF3		= 0x83,
		GAIN_COEFF4		= 0x84,
		GAIN_COEFF5		= 0x85,
		GAIN_COEFF6		= 0x86,
		GAIN_COEFF7		= 0x87,
		GAIN_COEFF8		= 0x88,
		GAIN_COEFF9		= 0x89,
		GAIN_COEFF10	= 0x8A,
		GAIN_COEFF11	= 0x8B,
		GAIN_COEFF12	= 0x8C,
		GAIN_COEFF13	= 0x8D,
		GAIN_COEFF14	= 0x8E,
		GAIN_COEFF15	= 0x8F,
		OFFSET_COEFF00	= 0x90,
		OFFSET_COEFF01	= 0x91,
		OFFSET_COEFF02	= 0x92,
		OFFSET_COEFF03	= 0x93,
		OFFSET_COEFF04	= 0x94,
		OFFSET_COEFF05	= 0x95,
		OFFSET_COEFF06	= 0x96,
		OFFSET_COEFF07	= 0x97,
		OFFSET_COEFF08	= 0x98,
		OFFSET_COEFF09	= 0x99,
		OFFSET_COEFF010	= 0x9A,
		OFFSET_COEFF011	= 0x9B,
		OFFSET_COEFF012	= 0x9C,
		OFFSET_COEFF013	= 0x9D,
		OFFSET_COEFF014	= 0x9E,
		OFFSET_COEFF015	= 0x9F,
		OPT_COEF0		= 0xA0,
		OPT_COEF1		= 0xA1,
		OPT_COEF2		= 0xA2,
		OPT_COEF3		= 0xA3,
		OPT_COEF4		= 0xA4,
		OPT_COEF5		= 0xA5,
		OPT_COEF6		= 0xA6,
		OPT_COEF7		= 0xA7,
		OPT_COEF8		= 0xA8,
		OPT_COEF9		= 0xA9,
		OPT_COEF10		= 0xAA,
		OPT_COEF11		= 0xAB,
		OPT_COEF12		= 0xAC,
		OPT_COEF13		= 0xAD,
		SERIAL1			= 0xAE,
		SERIAL0			= 0xAF,
	};

	template<class T>
	friend T operator+( const T& rn, const int n )
	{
		return T( static_cast<uint16_t>( rn ) + n );
	}

	template<class T>
	friend T operator+( const int n, const T& rn )
	{
		return T( n + static_cast<uint16_t>( rn ) );
	}

	enum Command : uint16_t {
		CMD_CH0 			= 0x0000,
		CMD_CH1 			= 0x0001,
		CMD_CH2 			= 0x0002,
		CMD_CH3 			= 0x0003,
		CMD_CH4 			= 0x0004,
		CMD_CH5 			= 0x0005,
		CMD_CH6 			= 0x0006,
		CMD_CH7 			= 0x0007,
		CMD_CH8 			= 0x0008,
		CMD_CH9 			= 0x0009,
		CMD_CH10 			= 0x000A,
		CMD_CH11 			= 0x000B,
		CMD_CH12 			= 0x000C,
		CMD_CH13 			= 0x000D,
		CMD_CH14 			= 0x000E,
		CMD_CH15 			= 0x000F,
		CMD_ABORT 			= 0x0010,
		CMD_END				= 0x0011,
		CMD_CLEAR_ALARM		= 0x0012,
		CMD_CLEAR_DATA		= 0x0013,
		CMD_RESET			= 0x0014,
		CMD_CLEAR_REG		= 0x0015,
		CMD_RELOAD			= 0x0016,
		TBD					= 0x0017,
		CMD_SS				= 0x2000,
		CMD_SC				= 0x2001,
		CMD_MM				= 0x2002,
		CMD_MC				= 0x2003,
		CMD_MS				= 0x2004,
		CMD_BURST_DATA		= 0x2005,
		CMD_CALC_CRC_CONFG	= 0x2006,
		CMD_CALC_CRC_COEF	= 0x2007,
		CMD_CALC_CRC_FAC	= 0x2008,
	};

	/** Command
	 *	
	 * @param com "Comand" type or uint16_t value
	 */
	void		command( uint16_t com );

	/** Write register
	 *
	 *	Writes register. Register width is selected by reg type (Register16 ot Register24)
	 * @param reg register specified by Register16 member
	 */
	void		reg( Register16 r, uint16_t value );

	/** Write register
	 *
	 *	Writes register. Register width is selected by reg type (Register16 ot Register24)
	 * @param reg register specified by Register24 member
	 */
	void		reg( Register24 r, uint32_t value );

	/** Read register
	 *
	 *	Reads register. Register width is selected by reg type (Register16 ot Register24)
	 * @param reg register specified by Register16 member
	 * @return readout value
	 */
	uint16_t	reg( Register16 r );

	/** Read register
	 *
	 *	Reads register. Register width is selected by reg type (Register16 ot Register24)
	 * @param reg register specified by Register24 member
	 * @return readout value
	 */
	uint32_t	reg( Register24 r );
	
	/** Register bit operation
	 *
	 *	overwrite bits i a register
	 * @param reg register specified by Register16 or Register24 member
	 * @param mask mask bits
	 * @param reg value to over write
	 */
	template<typename T>
	uint32_t	bit_op( T rg, uint32_t mask, uint32_t value )
	{
		uint32_t v	= reg( rg );

		v	&= mask;
		v	|= value & ~mask;

		reg( rg, v );
		
		return v;
	}

	/** Read part_number
	 *
	 * @return 0x13388B40 
	 */
	uint32_t	part_number( void );

	/** Read rivision number
	 *
	 * @return PN0 register value & 0xF
	 */
	uint8_t	revision_number( void );

	/** Read serial number
	 *
	 * @return serial number
	 */
	uint64_t	serial_number( void );

	/** Die temperature
	 *
	 * @return die temperature in celsius
	 */
	float	temperature( void );
private:
	DigitalIn	pin_nINT;
	DigitalIn	pin_DRDY;
	DigitalOut	pin_SYN;
	DigitalOut	pin_nRESET;
};

class NAFE13388 : public NAFE13388_Base
{
public:	
	/** Constructor to create a NAFE13388 instance */
	NAFE13388( SPI& spi, int nINT = D2, int DRDY = D3, int SYN = D5, int nRESET = D6 );

	/** Destractor */
	virtual ~NAFE13388();
};

class NAFE13388_UIM : public NAFE13388_Base
{
public:	
	/** Constructor to create a NAFE13388 instance */
	NAFE13388_UIM( SPI& spi, int nINT = D3, int DRDY = D4, int SYN = D6, int nRESET = D7 );

	/** Destractor */
	virtual ~NAFE13388_UIM();
};


#endif //	ARDUINO_AFE_DRIVER_H
