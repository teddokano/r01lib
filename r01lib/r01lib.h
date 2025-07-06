/*
 *  @author Tedd OKANO
 *
 *  Released under the MIT license License
 */

#ifndef R01LIB_R01LIB_H
#define R01LIB_R01LIB_H

extern "C" {
#include	"fsl_debug_console.h"
}

#define		MCUXPRESSO_V25_06_OR_LATER 

#if (defined(SDK_DEBUGCONSOLE) && (SDK_DEBUGCONSOLE == DEBUGCONSOLE_REDIRECT_TO_SDK))
#ifdef	MCUXPRESSO_V25_06_OR_LATER
#else	//	MCUXPRESSO_V25_06_OR_LATER
#define printf	DbgConsole_Printf
#define scanf	DbgConsole_Scanf
#define putchar	DbgConsole_Putchar
#define getchar	DbgConsole_Getchar
#endif	//	MCUXPRESSO_V25_06_OR_LATER
#else
#define		SEMIHOST_OPERATION
#endif

#include	"i3c.h"
#include	"i2c.h"
#include	"spi.h"
#include	"io.h"
#include	"Ticker.h"
#include	"InterruptIn.h"
#include	"BusInOut.h"
#include	"mcu.h"

#include	<iostream>
#include	<iomanip>

#endif // R01LIB_R01LIB_H
