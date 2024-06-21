/*
 *  @author Tedd OKANO
 *
 *  Released under the MIT license License
 */

#ifndef R01LIB_TICKER_H
#define R01LIB_TICKER_H

#include <functional>

extern "C" {
#include	"fsl_utick.h"
}

/** Ticker class
 *	
 *  @class Ticker
 *
 *	A class for demonstrating SPI bus
 *
 * @note only 1 ticker can be used in this version
 */
class Ticker
{	
public:
	enum{
		repeat	= kUTICK_Repeat
	};
	
	/** Create a Ticker instance */
	Ticker();
	
	/** Destractor to freeing SPI resource */
	~Ticker();
	
	/** Register callback function
	 *
	 * @param callback callback function
	 * @param sec periodic cycle to call the callback
	 */
	void	attach( utick_callback_t callback, float sec );

private:
	UTICK_Type	*utick_type;
};

#endif // R01LIB_TICKER_H
