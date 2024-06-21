/*
 *  @author Tedd OKANO
 *
 *  Released under the MIT license License
 */

#ifndef R01LIB_MCU_H
#define R01LIB_MCU_H

#include "r01lib.h"

void	init_mcu( void );
void	wait( float delayTime_sec );
void 	panic( const char *s );


#endif // R01LIB_MCU_H
