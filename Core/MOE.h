/*
 *  MOE.h
 *
 * Basic MOE lib header
 * 
 */

#ifndef INCLUDE_MOE_H
#define INCLUDE_MOE_H

#include "Memory.h"
#include "Type.h"

#if 1
#include "Log.h"
#else
#define MOELogE printf
#define MOELogW printf
#define MOELogI printf
#define MOELogD printf
#endif

#endif // INCLUDE_MOE_H

