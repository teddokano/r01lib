/*
 *  @author Tedd OKANO
 *
 *  Released under the MIT license License
 */

#ifndef R01LIB_IO_H
#define R01LIB_IO_H

#include	"fsl_port.h"
#include	"pin_mux.h"
#include	"clock_config.h"
#include	"board.h"
#include	"obj.h"

#define	PIN_OUTPUT			kGPIO_DigitalOutput
#define	PIN_INPUT			kGPIO_DigitalInput

#define	DISABLED_GPIO		-1
#define	DISABLED_PIN		0xFF

#ifdef	CPU_MCXN947VDF
	#define A0_GPIO				DISABLED_GPIO
	#define A1_GPIO				DISABLED_GPIO
	#define A2_GPIO				0
	#define A3_GPIO				0
	#define A4_GPIO				0
	#define A5_GPIO				0
	#define D0_GPIO				4
	#define D1_GPIO				4
	#define D2_GPIO				0
	#define D3_GPIO				1
	#define D4_GPIO				0
	#define D5_GPIO				1
	#define D6_GPIO				1
	#define D7_GPIO				0
	#define D8_GPIO				0
	#define D9_GPIO				0
	#define D10_GPIO			0
	#define D11_GPIO			0
	#define D12_GPIO			0
	#define D13_GPIO			0
	#define D18_GPIO			4
	#define D19_GPIO			4
	#define SW2_GPIO			A5_GPIO
	#define SW3_GPIO			0
	#define	MB_AN_GPIO			DISABLED_GPIO
	#define	MB_RST_GPIO			1
	#define	MB_CS_GPIO			3
	#define	MB_SCK_GPIO			3
	#define	MB_MISO_GPIO		3
	#define	MB_MOSI_GPIO		3
	#define	MB_PWM_GPIO			3
	#define	MB_INT_GPIO			5
	#define	MB_RX_GPIO			1
	#define	MB_TX_GPIO			1
	#define	MB_SCL_GPIO			1
	#define	MB_SDA_GPIO			1
	#define	P3T_SDA_GPIO		1
	#define	P3T_SCL_GPIO		1

	#define A0_GPIO_PIN			DISABLED_PIN
	#define A1_GPIO_PIN			DISABLED_PIN
	#define A2_GPIO_PIN			14U
	#define A3_GPIO_PIN			22U
	#define A4_GPIO_PIN			15U
	#define A5_GPIO_PIN			23U
	#define D0_GPIO_PIN			3U
	#define D1_GPIO_PIN			2U
	#define D2_GPIO_PIN			29U
	#define D3_GPIO_PIN			23U
	#define D4_GPIO_PIN			30U
	#define D5_GPIO_PIN			21U
	#define D6_GPIO_PIN			2U
	#define D7_GPIO_PIN			31U
	#define D8_GPIO_PIN			28U
	#define D9_GPIO_PIN			10U
	#define D10_GPIO_PIN		27U
	#define D11_GPIO_PIN		24U
	#define D12_GPIO_PIN		26U
	#define D13_GPIO_PIN		25U
	#define D18_GPIO_PIN		0U
	#define D19_GPIO_PIN		1U
	#define SW2_GPIO_PIN		A5_GPIO_PIN
	#define SW3_GPIO_PIN		6
	#define	MB_AN_GPIO_PIN		DISABLED_PIN
	#define	MB_RST_GPIO_PIN		3
	#define	MB_CS_GPIO_PIN		23
	#define	MB_SCK_GPIO_PIN		21
	#define	MB_MISO_GPIO_PIN	22
	#define	MB_MOSI_GPIO_PIN	20
	#define	MB_PWM_GPIO_PIN		19
	#define	MB_INT_GPIO_PIN		7
	#define	MB_RX_GPIO_PIN		16
	#define	MB_TX_GPIO_PIN		17
	#define	MB_SCL_GPIO_PIN		1
	#define	MB_SDA_GPIO_PIN		0
	#define	P3T_SDA_GPIO_PIN	16
	#define	P3T_SCL_GPIO_PIN	17

	#define	RED					D9
	#define	GREEN				D10
	#define	BLUE				D6

#else // CPU_MCXN947VDF

	#define D0_GPIO				1
	#define D1_GPIO				1
	#define D2_GPIO				2
	#define D3_GPIO				3
	#define D4_GPIO				2
	#define D5_GPIO				3
	#define D6_GPIO				3
	#define D7_GPIO				3
	#define D8_GPIO				3
	#define D9_GPIO				3
	#define D10_GPIO			2
	#define D11_GPIO			2
	#define D12_GPIO			2
	#define D13_GPIO			2
	#define D18_GPIO			1
	#define D19_GPIO			1
	#define SW2_GPIO			3
	#define SW3_GPIO			1
	#define A0_GPIO				1
	#define A1_GPIO				1
	#define A2_GPIO				1
	#define A3_GPIO				2
	#define A4_GPIO				3
	#define A5_GPIO				3
	#define	MB_AN_GPIO			3
	#define	MB_RST_GPIO			3
	#define	MB_CS_GPIO			1
	#define	MB_SCK_GPIO			1
	#define	MB_MISO_GPIO		1
	#define	MB_MOSI_GPIO		1
	#define	MB_PWM_GPIO			3
	#define	MB_INT_GPIO			2
	#define	MB_RX_GPIO			3
	#define	MB_TX_GPIO			3
	#define	MB_SCL_GPIO			3
	#define	MB_SDA_GPIO			3

	#define	P3T_SDA_GPIO		1
	#define	P3T_SCL_GPIO		1

	#define D0_GPIO_PIN			4U
	#define D1_GPIO_PIN			5U
	#define D2_GPIO_PIN			4U
	#define D3_GPIO_PIN			0U
	#define D4_GPIO_PIN			5U
	#define D5_GPIO_PIN			12U
	#define D6_GPIO_PIN			13U
	#define D7_GPIO_PIN			1U
	#define D8_GPIO_PIN			15U
	#define D9_GPIO_PIN			14U
	#define D10_GPIO_PIN		6U
	#define D11_GPIO_PIN		13U
	#define D12_GPIO_PIN		16U
	#define D13_GPIO_PIN		12U
	#define D18_GPIO_PIN		8U
	#define D19_GPIO_PIN		9U
	#define SW2_GPIO_PIN		29U
	#define SW3_GPIO_PIN		7U
	#define A0_GPIO_PIN			10U
	#define A1_GPIO_PIN			12U
	#define A2_GPIO_PIN			13U
	#define A3_GPIO_PIN			0U
	#define A4_GPIO_PIN			31U
	#define A5_GPIO_PIN			30U

	#define	MB_AN_GPIO_PIN		30
	#define	MB_RST_GPIO_PIN		1
	#define	MB_CS_GPIO_PIN		3
	#define	MB_SCK_GPIO_PIN		1
	#define	MB_MISO_GPIO_PIN	2
	#define	MB_MOSI_GPIO_PIN	0
	#define	MB_PWM_GPIO_PIN		12
	#define	MB_INT_GPIO_PIN		5
	#define	MB_RX_GPIO_PIN		14
	#define	MB_TX_GPIO_PIN		15
	#define	MB_SCL_GPIO_PIN		27
	#define	MB_SDA_GPIO_PIN		28

	#define	P3T_SDA_GPIO_PIN	16
	#define	P3T_SCL_GPIO_PIN	17

	#define	RED					D5
	#define	GREEN				D6
	#define	BLUE				D3

#endif // CPU_MCXN947VDF

#define	I2C_SDA				D18
#define	I2C_SCL				D19

#define	SPI_CS				D10
#define	SPI_MOSI			D11
#define	SPI_MISO			D12
#define	SPI_SCLK			D13

/** pin names  */
enum { 	D0, D1, D2, D3, D4, D5, D6, D7, D8, D9, D10, D11, D12, D13, D18, D19,
		A0, A1, A2, A3, A4, A5, 
		SW2, SW3,
		MB_AN, MB_RST, MB_CS, MB_SCK, MB_MISO, MB_MOSI, MB_PWM, MB_INT, MB_RX, MB_TX, MB_SCL, MB_SDA, 
		I3C_SDA, I3C_SCL
};

#define	PIN_LED_OFF	true
#define	PIN_LED_ON	false


/** DigitalInOut class
 *	
 *  @class DigitalInOut
 *
 *	A class for operating GPIO easy
 */

class DigitalInOut: public Obj
{
public:
	enum	direction {
		INPUT	= kGPIO_DigitalInput, 
		OUTPUT	= kGPIO_DigitalOutput
	};
	/** Create a DigitalInOut instance with specified pins
	 *
	 * @param pin_num pin number
	 * @param direction (option) direction setting
	 * @param value (option) default value for output
	 */
	DigitalInOut( uint8_t pin_num, bool direction = kGPIO_DigitalInput, bool value = 0 );

	/** Destractor
	 */
	~DigitalInOut();
	
	/** Pin output seting
	 *
	 * @param value setting output
	 */
	void	value( bool value );

	/** Pin input state read
	 *
	 * @return pin state
	 */
	bool	value( void );
	
	/** Pin direction to set as output
	 */
	void	output( void );

	/** Pin direction to set as in
	 */
	void	input( void );

	/** Pin mux setting
	 * This interface is provided to other class drivers to change pin config dynamically
	 */
	void	pin_mux( int mux );
		
	/** A short hand for setting pins
	 */
	DigitalInOut&	operator=( bool v );
	DigitalInOut&	operator=( DigitalInOut& rhs );

	/** A short hand for reading pins
	 */
	operator	bool();

protected:
	void direction( bool dir );
	
	uint8_t		_pn; 
	GPIO_Type	*gpio_n;
	PORT_Type	*port_n;
	uint8_t		gpio_pin; 
	
private:
	bool 	_dir; 
	bool 	_value;
};

/** DigitalOut class
 *	
 *  @class DigitalOut
 *
 *	A class for operating GPIO easy
 */

class DigitalOut : public DigitalInOut
{
public:
	using DigitalInOut::operator=;

	DigitalOut( uint8_t pin_num, bool value = 0 );
	~DigitalOut();
};

/** DigitalIn class
 *	
 *  @class DigitalIn
 *
 *	A class for operating GPIO easy
 */

class DigitalIn : public DigitalInOut
{
public:
	using DigitalInOut::operator=;

	DigitalIn( uint8_t pin_num );
	~DigitalIn();
};

#endif // R01LIB_IO_H
