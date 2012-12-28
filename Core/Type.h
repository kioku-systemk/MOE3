//-
//	MOEType
//  -- Shift-JIS return CR+LF -- 
//-

#ifndef INCLUDE_MOETYPE_H
#define INCLUDE_MOETYPE_H

// 型定義はC++であることが前提となっているものがあるので、C++コンパイラでない場合はエラーとする
#ifndef __cplusplus
	#error "CPP COMPILER REQUIRED."
#endif

//
// 組み込み型のMOE用定義
//
typedef char                s8;
typedef signed short        s16;
typedef signed int          s32;
#if defined (_MSC_VER)
    typedef signed __int64  s64;
#else
    typedef	long long int   s64;
#endif
typedef unsigned char       u8;
typedef unsigned short      u16;
typedef unsigned int        u32;
#if defined (_MSC_VER)
    typedef unsigned __int64 u64;
#else
    typedef	unsigned long long int u64;
#endif
typedef float               f32;
typedef double              f64;
typedef bool                b8;  //8bit boolean
typedef int                 b32; //32bit boolean

// SAFE_DELETE系
/*
#ifndef SAFE_DELETE
	#define SAFE_DELETE(p) do{ if(p){ delete p; p=NULL; } }while(0)
#endif
#ifndef SAFE_DELETE_ARRAY
	#define SAFE_DELETE_ARRAY(p) do{ if(p){ delete[] p; p=NULL; } }while(0)
#endif

#ifndef MOE_DELETE
	#define MOE_DELETE(p) do{ if(p){ delete p; p=NULL; } }while(0)
#endif
#ifndef MOE_DELETE_ARRAY
	#define MOE_DELETE_ARRAY(p) do{ if(p){ delete[] p; p=NULL; } }while(0)
#endif
#ifndef MOE_RELEASE
	#define MOE_RELEASE(p) do{ if(p){ p->release(); p=NULL; } }while(0)
#endif
*/

// プラットフォーム識別コード
// Windows
#if defined(_WIN32)
#if defined(_WIN64)
	#define MOE_PLATFORM_WIN32 (0)
	#define MOE_PLATFORM_WIN64 (1)
#else
	#define MOE_PLATFORM_WIN32 (1)
	#define MOE_PLATFORM_WIN64 (0)
#endif
// ビルド対象ウィンドウズバージョン
#undef _WIN32_WINNT
#undef _WIN32_WINDOWS
#define _WIN32_WINNT    0x0400	//Win2k or later
#define _WIN32_WINDOWS  0x0500	//Win98 or later


// MacOSX(intel)
#elif defined(__APPLE__) || defined(MACOSX)
#if defined(__i386__)
	#define MOE_PLATFORM_MAC32 (1)
	#define MOE_PLATFORM_MAC64 (0)
#elif defined(__x86_64__)
	#define MOE_PLATFORM_MAC32 (0)
	#define MOE_PLATFORM_MAC64 (1)
#else
	#define MOE_PLATFORM_MAC64 (1)
#endif

// Linux
#elif defined(LINUX)
#if defined(__i386__)
	#define MOE_PLATFORM_LIN32 (1)
	#define MOE_PLATFORM_LIN64 (0)
#elif defined(__x86_64__)
	#define MOE_PLATFORM_LIN32 (0)
	#define MOE_PLATFORM_LIN64 (1)
#endif


#endif

// Platform OS Type
#if MOE_PLATFORM_WIN32 || MOE_PLATFORM_WIN64
	#define MOE_PLATFORM_WINDOWS (1)
	#define MOE_PLATFORM_MACOSX  (0)
	#define MOE_PLATFORM_LINUX   (0)
#elif MOE_PLATFORM_MAC32 || MOE_PLATFORM_MAC64
	#define MOE_PLATFORM_WINDOWS (0)
	#define MOE_PLATFORM_MACOSX  (1)
	#define MOE_PLATFORM_LINUX   (0)
#elif MOE_PLATFORM_LIN32 || MOE_PLATFORM_LIN64
	#define MOE_PLATFORM_WINDOWS (0)
	#define MOE_PLATFORM_MACOSX  (0)
	#define MOE_PLATFORM_LINUX   (1)
#endif

#if MOE_PLATFORM_WINDOWS
	#include <windows.h>
#endif


// ユーティリティ
#define DEG2RAD(_f32_deg_) (3.141592f / 180.f * (_f32_deg_))
#define RAD2DEG(_f32_rad_) ((_f32_rad_) * 180.0f / 3.141592f)
#define VEC3R2D(_vec3_rad_) do{ (_vec3_rad_).x = RAD2DEG((_vec3_rad_).x); (_vec3_rad_).y = RAD2DEG((_vec3_rad_).y); (_vec3_rad_).z = RAD2DEG((_vec3_rad_).z); }while(0)

#endif /* INCLUDE_MOETYPE_H */
