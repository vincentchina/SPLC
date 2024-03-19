#ifndef __DLLPREHEAD_H__
#define __DLLPREHEAD_H__

#ifdef __cplusplus
extern "C"{
#endif

#ifdef LIBX_EXPORTS
#define LIBX_API __declspec(dllexport)
#else
#define LIBX_API __declspec(dllimport)
#endif

//#define LIBX_API __declspec(dllimport)

#ifdef __cplusplus
}
#endif
#endif
