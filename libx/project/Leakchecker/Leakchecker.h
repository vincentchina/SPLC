#pragma once

#include <windows.h>

/* 非MFC版本需要在在程序结束调用_CrtDumpMemoryLeaks()*/

#define DEBUG_NEW new(__FILE__, __LINE__) 

void * __cdecl operator new[](size_t); 
void* __cdecl operator new[](size_t nSize, LPCSTR lpszFileName, int nLine); 
void __cdecl operator delete[](void* p, LPCSTR lpszFileName, int nLine); 
void __cdecl operator delete[](void *); 

void * __cdecl operator new(size_t); 
void* __cdecl operator new(size_t nSize, LPCSTR lpszFileName, int nLine); 
void __cdecl operator delete(void* p, LPCSTR lpszFileName, int nLine); 
void __cdecl operator delete(void *);  