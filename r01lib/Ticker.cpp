/*
 * Copyright 2024 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

/*  Standard C Included Files */

extern "C" {
#include "fsl_common.h"
#include "fsl_debug_console.h"
}

#include	"Ticker.h"

//Timer::Timer( int type, uint32_t period, const std::function<void()>& callback )
Ticker::Ticker()
	: utick_type( UTICK0 )
{
}

Ticker::~Ticker() {}

void Ticker::attach( utick_callback_t callback, float sec )
{
	UTICK_SetTick( utick_type, kUTICK_Repeat, (uint32_t)(sec * 1000000.0) - 1, callback );
}
