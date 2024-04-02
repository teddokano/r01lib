/*
 * Copyright 2024 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef R01LIB_BASE_OBJ_H
#define R01LIB_BASE_OBJ_H

class Obj
{
public:
	Obj( bool done = false );
	~Obj();
private:
	static bool	init_done;
};

#endif // R01LIB_BASE_OBJ_H
