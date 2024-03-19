#ifndef __BASE_H__
#define __BASE_H__

#ifdef __cplusplus
extern "C"{
#endif

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
typedef unsigned short uint16;
typedef short int16;
typedef unsigned int uint32;
typedef int int32;
typedef short api_res;
typedef unsigned short mbyte;
typedef unsigned char uint8,byte;

#ifdef _DEBUG
#pragma comment( lib, "..\\..\\Debug\\libx" )
#else
#pragma comment( lib, "..\\..\\Release\\libx" )
#endif

#include "plcInsSys\plcInsSys.h"
#include "common/misc.h"


#ifdef __cplusplus
}
#endif
#endif	