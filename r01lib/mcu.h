/*
 * Copyright 2024 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef R01LIB_MCU_H
#define R01LIB_MCU_H

#include "r01lib.h"

void	init_mcu( void );
void	wait( float delayTime_sec );
void 	panic( const char *s );


#endif // R01LIB_MCU_H
