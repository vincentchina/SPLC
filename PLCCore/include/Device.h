#ifndef __DEVICE_H__
#define __DEVICE_H__

#ifdef __cplusplus
extern "C"{
#endif

//必须指定16位和32位数据
//mbyte根据机器而定,可能一个mbyte不是8位，所以需要修改读取文件部分
//使用linux思想，没有消息就是最好的消息，成功用0返回

#if defined( _WIN32 )
	#include "../Win32/win32.h"
#elif defined( _STM32 )
	#include "../stm32/stm32.h"
#elif defined( _DSP )
	#include "../Win32/win32.h"
#else
	#error "undefine device!"
#endif

#ifndef _DPRINTF
#define _DPRINTF( ... )
#endif

//定义的错误代码
#define RES_OK		1
#define RES_ERR		-1

#ifdef __cplusplus
}
#endif
#endif


