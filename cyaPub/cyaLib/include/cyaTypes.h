#ifndef __CYA_TYPES_H__
#define __CYA_TYPES_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

#include "cyaBase.h"

#if !defined(_T)
	#if defined(_UNICODE) || defined(UNICODE)
		#define _T(s)			L ## s
	#else
		#define _T(s)			s
	#endif
#endif

#undef INADDR_NONE
#define INADDR_NONE				(DWORD)0xffffffff

#undef INADDR_ANY
#define INADDR_ANY				(DWORD)0x00000000

#undef ADDR_ANY
#define ADDR_ANY				INADDR_ANY

#undef INADDR_LOOPBACK
#define INADDR_LOOPBACK			(DWORD)0x7f000001

#define INADDR_LOOPBACK_NET		htonl(INADDR_LOOPBACK)

#undef INADDR_BROADCAST
#define INADDR_BROADCAST		(DWORD)0xffffffff

/* bool SockValid(SOCKET sock); */
#define SockValid(sock)			((INT_PTR)(sock) >= 0)

/* bool FDValid(FD_TYPE fd); */
#define FDValid(fd)				((INT_PTR)(fd) >= 0)

// /* 离散SOCKET读写结构类型 */
// typedef struct
// {
// 	void* buf;
// 	int len;
// } SockReadBuf;
// typedef struct
// {
// 	const void* data;
// 	int len;
// } SockWriteBuf;

#ifndef MAX_PATH
	#define MAX_PATH	260
#endif
typedef char PathNameA[MAX_PATH];
typedef WCHAR PathNameW[MAX_PATH];
typedef TCHAR PathName[MAX_PATH];

/* 描述时间毫秒数(64位)，没有时区之分，即没有系统时间和本地时间的区分 */
/* 绝对时间：[MIN_UTC_MSEL, MAX_UTC_MSEL] */
/* 相对时间：[-MAX_UTC_MSEL, MAX_UTC_MSEL] */
typedef INT64 LTMSEL;

/* [GMT: 1970/01/01 00:00:00; BEIJING: 1970/01/01 08:00:00] */
#define MIN_UTC_MSEL			(LTMSEL)(0)
/* [GMT: 2038/01/18 16:00:00; BEIJING: 2038/01/19 00:00:00] */
#define MAX_UTC_MSEL			((LTMSEL)0x7fff6200 * 1000)

/* 非法UTC时间 */
#define INVALID_UTC_MSEL		(LTMSEL)(-1)

#if defined(WIN32)
	typedef HANDLE OSThread;
#elif defined(__linux__)
	typedef pthread_t OSThread;
#endif
#ifndef OSTHREADID_DEFINED
	#define OSTHREADID_DEFINED
	#if defined(WIN32)
		typedef DWORD OSThreadID;
	#elif defined(__linux__)
		typedef pthread_t OSThreadID;
	#endif
#endif

typedef void* OSThreadRealID;

#define INVALID_OSTHREAD			((OSThread)0)
#define INVALID_OSTHREADID			((OSThreadID)0)
#define INVALID_OSTHREAD_REAL_ID	((OSThreadRealID)0)

#if defined(WIN32)
	typedef DWORD OSProcessID;
#else
	typedef int OSProcessID;
#endif

#define INVALID_OSPROCESSID			((OSProcessID)0)

#if defined(WIN32)
	typedef HANDLE OSHandle;
	#define INVALID_OS_HANDLE	((OSHandle)NULL)
	#define OSHandleValid(hd)	(INVALID_OS_HANDLE != (hd))
#elif defined(__linux__)
	typedef int OSHandle;
	#define INVALID_OS_HANDLE	((OSHandle)-1)
	#define OSHandleValid(hd)	((hd) >= 0)
#endif

#if defined(__linux__)
	#define		WSAGetLastError()	errno
	#define		GetLastError()		errno
#endif

/* 小字序环境判断 */
BOOL IsSmallEndian();

#if defined(WIN32)
	/* 获取Windows版本信息 */
	typedef enum enumWindowsVersion
	{
		Invalid_WinVer = -1,
		Unknown_WinVer = 0,
		Win32s,
		Windows_95,
		Windows_98,
		Windows_Me,
		Windows_NT,
		Windows_2000,
		Windows_XP,
		Windows_Server_2003,
		Windows_Server_2003_R2,
		Windows_Longhorn,
		Windows_Vista,
		Windows_Server_2008,
		Windows_7,
		Windows_Server_2008_R2,
		Windows_8,
	} WindowsVersion;
#endif

typedef enum enumOSExeBits
{
	OS_EXE_32BIT,
	OS_EXE_64BIT,
	OS_EXE_UNKNOWN
} OSExeBits;

#ifdef __cplusplus
	/// 线程池任务基类
	struct ITBaseTask
	{
		ITBaseTask* m_nextPoolTask; /// 连接到下一个任务
		virtual ~ITBaseTask() {}
		/// 线程池调度到这个任务时执行
		virtual void OnPoolTask() = 0;
	};

	/// 基础timer相关类型
	struct tagOSTimerID { void* dummy; };
	typedef tagOSTimerID* TTimerID;
	typedef void(*fnTimerCallback)(void* param, TTimerID id);

#endif /* #ifdef __cplusplus */

/* 基本内存分配函数指针类型 */
typedef void* (*fnMemAllocFuncPtr)(INT_PTR);
typedef void(*fnMemFreeFuncPtr)(void*);

/* tstring */
#ifdef __cplusplus
	#if defined(_UNICODE) || defined(UNICODE)
		#ifndef NO_WSTRING
			typedef std::wstring tstring;
		#endif
	#else
		typedef std::string tstring;
	#endif
	#define empty_tstring	tstring()
#endif

/* struct IPPADDR */
#ifdef __cplusplus
	#if defined(_MSC_VER)
		#pragma warning(disable: 4201)	/* nonstandard extension used : nameless struct/union */
	#endif
	#pragma pack(push, 1)
	// 用于统一记录IP和port，方便统一使用、管理、调试
	struct IPPADDR
	{
		union
		{
			struct
			{
				union
				{
					DWORD ip_; // 如果不作特别说明，ip_使用网络字节序
					struct
					{
						BYTE bip0_, bip1_, bip2_, bip3_;
					};
				};
				union
				{
					INT32 port_; // 如果不作特别说明，port_使用本地字节序
					BYTE bport_[sizeof(INT32) / sizeof(BYTE)];
				};
			};
			INT64 i64_;
		};

		IPPADDR() : ip_(INADDR_NONE), port_(-1) {}
		IPPADDR(DWORD ip, int port) : ip_(ip), port_((INT32)port) {}

		IPPADDR(const char* ippStr, int default_port); // 未实现
		IPPADDR(const WCHAR* ippStr, int default_port);

		BOOL IsValid() const
		{	return INADDR_NONE != ip_ && port_ >= 0;	}

		bool operator< (const IPPADDR& r) const
		{	return i64_ < r.i64_;	}
		bool operator<= (const IPPADDR& r) const
		{	return i64_ <= r.i64_;	}
		bool operator>(const IPPADDR& r) const
		{	return i64_ > r.i64_;	}
		bool operator>= (const IPPADDR& r) const
		{	return i64_ >= r.i64_;	}
		bool operator== (const IPPADDR& r) const
		{	return i64_ == r.i64_;	}
		bool operator!= (const IPPADDR& r) const
		{	return i64_ != r.i64_;	}
	};
	#pragma pack(pop)
	#if defined(_MSC_VER)
		static INT64 volatile sg_INVALID_IPPADDR = -1;
		#define INVALID_IPPADDR		((const IPPADDR&)sg_INVALID_IPPADDR)
	#else
		#define INVALID_IPPADDR		IPPADDR()
	#endif
#endif
/* 使用OS_FILENAME_MATCH_CASE以解决平台路径名大小写区分差异 */
#if defined(WIN32)
	#define OS_FILENAME_MATCH_CASE	false
#else
	#define OS_FILENAME_MATCH_CASE	true
#endif

/* 默认trim字符集 */
#define STRDK_TRIM_CHARS_A		"\x20\r\n\t"
#define STRDK_TRIM_CHARS_W		L"\x20\r\n\t"
#define STRDK_TRIM_CHARS		_T("\x20\r\n\t")

/* 默认路径保留字符集 */
#define PATH_FILE_INVALID_CHARS_A		"<>:|/\\\""
#define PATH_FILE_INVALID_CHARS_W		L"<>:|/\\\""
#define PATH_FILE_INVALID_CHARS			_T("<>:|/\\\"")

//路径分割符
#if defined(WIN32)
	#define OS_DIR_SEP_CHAR_A		'\\'
	#define OS_DIR_SEP_CHAR_STR_A	"\\"
	#define OS_DIR_SEP_CHAR_W		L'\\'
	#define OS_DIR_SEP_CHAR_STR_W	L"\\"
#else
	#define OS_DIR_SEP_CHAR_A		'/'
	#define OS_DIR_SEP_CHAR_STR_A	"/"
	#define OS_DIR_SEP_CHAR_W		L'/'
	#define OS_DIR_SEP_CHAR_STR_W	L"/"
#endif

#if defined(_UNICODE) || defined(UNICODE)
	#define OS_DIR_SEP_CHAR			OS_DIR_SEP_CHAR_W
	#define OS_DIR_SEP_CHAR_STR		OS_DIR_SEP_CHAR_STR_W
#else
	#define OS_DIR_SEP_CHAR			OS_DIR_SEP_CHAR_A
	#define OS_DIR_SEP_CHAR_STR		OS_DIR_SEP_CHAR_STR_A
#endif

#ifndef INVALID_SET_FILE_POINTER
	#define INVALID_SET_FILE_POINTER (DWORD)-1
#endif

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+++++++++++++++++++++++ 内联函数实现 +++++++++++++++++++++++*/
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
union endian_check_union_
{
	WORD w;
	BYTE ch[2];
};

inline BOOL IsSmallEndian()
{
	union endian_check_union_ check;
	check.w = 0x1234;
	return (0x34 == check.ch[0]);
}

template <typename TYPE>
inline TYPE xs_ntoh(TYPE v)
{
	if (!IsSmallEndian())	//大端序直接返回
		return v;
	const int l = SIZEOF(TYPE);
	TYPE r;
	BYTE* p = (BYTE*)&r;
	for (int i = 0; i < l; ++i)
		p[i] = ((BYTE*)&v)[l - 1 - i];

	return r;
}

template <typename TYPE>
inline TYPE xs_hton(TYPE v)
{
	if (!IsSmallEndian())	//大端序直接返回
		return v;
	const int l = SIZEOF(TYPE);
	TYPE r;
	BYTE* p = (BYTE*)&r;
	for (int i = 0; i < l; ++i)
		p[i] = ((BYTE*)&v)[l - 1 - i];

	return r;
}

//字节按位逆序
inline BYTE xs_byte_converse(BYTE c)
{
	c = (c & 0xaa) >> 1 | (c & 0x55) << 1;
	c = (c & 0xcc) >> 2 | (c & 0x33) << 2;
	c = (c & 0xf0) >> 4 | (c & 0x0f) << 4;
	return c;
}

// 符号切换，即把最高位取反
// 一般用法是把返回值赋给TYPE的符号相反类型，如 BYTE bt = SignSwitch(ch); char ch = SignSwitch(bt);
//		即是如下映射变换 BYTE[0, 255] <---> char[-128, 127]
// T_INT SIGN_SWITCH(T_INT, v);『如果v为编译期常量，则SIGN_SWITCH(T_INT, v)是编译期常量』
#define SIGN_SWITCH(T_INT, v) ((T_INT)((T_INT)(v) ^ ((T_INT)1 << (SIZEOF(T_INT)*8-1))))
template<typename T_INT>
inline T_INT SignSwitch(T_INT v)
{	return SIGN_SWITCH(T_INT, v);	}

// 类型TYPE的符号类型判断
// bool IS_SIGNED_TYPE(TYPE);『编译期常量』
#define IS_SIGNED_TYPE(TYPE) ((TYPE)((TYPE)0 - 1) < 0)

//-------------------------------------------------------------------------//
// 向上规整，如果v为指针的使用方法 p2 = (void*)AlignTo((UINT)p, (UINT)align)
#define ALIGN_TO(v, align)	(((v) + (align)-1) / (align) * (align))
template<typename TYPE, typename T2>
inline TYPE AlignTo(TYPE v, T2 align)
{	ASSERT(v >= 0 && align > 0); return ALIGN_TO(v, align);	}

// 向下规整
#define ALIGN_BACK(v, align)	((v) / (align) * (align))
template<typename TYPE, typename T2>
inline TYPE AlignBack(TYPE v, T2 align)
{	ASSERT(v >= 0 && align > 0); return ALIGN_BACK(v, align);	}

// 向上对齐到2的整数次幂
template<typename TYPE>
inline TYPE Align2Power(TYPE v)
{
	ASSERT(v >= 0);
	TYPE v2 = v;
	int bits = sizeof(TYPE) * 8;
	if (IS_SIGNED_TYPE(TYPE))
		bits -= 1;
	for (int i = 0; i<bits; ++i)
	{
		v2 = (TYPE)((TYPE)1 << i);
		if (v2 >= v)
			break;
	}
	ASSERT(v2 >= v); // 若assert失败，说明v值太大，其对齐2的整数次幂值，类型TYPE无法表示
	return v2;
}

//-------------------------------------------------------------------------//
// 得到int类型的最大值
// T_INT GET_INT_MAX_VALUE(T_INT);『编译期常量』
#define GET_INT_MAX_VALUE(T_INT)	(IS_SIGNED_TYPE(T_INT) ? SIGN_SWITCH(T_INT, (T_INT)-1) : (T_INT)-1)

// 得到int类型的最小值
// T_INT GET_INT_MIN_VALUE(T_INT);『编译期常量』
#define GET_INT_MIN_VALUE(T_INT)	((T_INT)~GET_INT_MAX_VALUE(T_INT))

// 得到类型的最大、最小值
namespace xmm
{
	template<typename TYPE> struct TypeMaxValue
	{
		static inline TYPE MaxValue()
		{	return GET_INT_MAX_VALUE(TYPE);	}
		static inline TYPE MinValue()
		{	return GET_INT_MIN_VALUE(TYPE);	}
	};
#define special_typemaxvalue_(TYPE, ma, mi)	\
	template<> struct TypeMaxValue<TYPE>	\
	{	static inline TYPE MaxValue()	{	return (TYPE)ma;	}	\
	static inline TYPE MinValue()	{	return (TYPE)mi;	}	}
	special_typemaxvalue_(bool, true, false);
	special_typemaxvalue_(float, 3.4E38, 3.4E-38);
	special_typemaxvalue_(double, 1.2E308, 1.2E-308);
#undef special_typemaxvalue_
}

#define GetTypeMaxValue(TYPE) (xmm::TypeMaxValue<TYPE >::MaxValue())
#define GetTypeMinValue(TYPE) (xmm::TypeMaxValue<TYPE >::MinValue())

#endif