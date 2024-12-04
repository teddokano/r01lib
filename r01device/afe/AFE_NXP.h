/** NXP Analog Front End class library for MCX
 *
 *  @class   NAFE13388
 *  @author  Tedd OKANO
 *
 *  Copyright: 2023 - 2024 Tedd OKANO
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
	NAFE13388_Base( SPI& spi );

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
	virtual void reset();
	
	/** Board initialization (initializing control pin state)
	 *
	 * @param _pin_nINT pin number for nINT
	 * @param _pin_DRDY pin number for DRDY
	 * @param _pin_SYN pin number for SYN
	 * @param _pin_nRESET pin number for nRESET
	 */
	virtual void board_init( int _pin_nINT, int _pin_DRDY, int _pin_SYN, int _pin_nRESET );

	/** Configure logical channel
	 *
	 * @param ch logical channel number (0 ~ 15)
	 * @param cc0	16bit value to be set CH_CONFIG0 register (0x20)
	 * @param cc1	16bit value to be set CH_CONFIG1 register (0x21)
	 * @param cc2	16bit value to be set CH_CONFIG2 register (0x22)
	 * @param cc3	16bit value to be set CH_CONFIG3 register (0x23)
	 */
	virtual void logical_ch_config( int ch, uint16_t cc0, uint16_t cc1, uint16_t cc2, uint16_t cc3 );

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
};

class NAFE13388 : public NAFE13388_Base
{
public:	
	/** Constructor to create a NAFE13388 instance */
	NAFE13388( SPI& spi );

	/** Destractor */
	virtual ~NAFE13388();
};

class NAFE13388_UIM : public NAFE13388_Base
{
public:	
	/** Constructor to create a NAFE13388 instance */
	NAFE13388_UIM( SPI& spi );

	/** Destractor */
	virtual ~NAFE13388_UIM();
};


#endif //	ARDUINO_AFE_DRIVER_H
