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

// /* ��ɢSOCKET��д�ṹ���� */
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

/* ����ʱ�������(64λ)��û��ʱ��֮�֣���û��ϵͳʱ��ͱ���ʱ������� */
/* ����ʱ�䣺[MIN_UTC_MSEL, MAX_UTC_MSEL] */
/* ���ʱ�䣺[-MAX_UTC_MSEL, MAX_UTC_MSEL] */
typedef INT64 LTMSEL;

/* [GMT: 1970/01/01 00:00:00; BEIJING: 1970/01/01 08:00:00] */
#define MIN_UTC_MSEL			(LTMSEL)(0)
/* [GMT: 2038/01/18 16:00:00; BEIJING: 2038/01/19 00:00:00] */
#define MAX_UTC_MSEL			((LTMSEL)0x7fff6200 * 1000)

/* �Ƿ�UTCʱ�� */
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

/* С���򻷾��ж� */
BOOL IsSmallEndian();

#if defined(WIN32)
	/* ��ȡWindows�汾��Ϣ */
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
	/// �̳߳��������
	struct ITBaseTask
	{
		ITBaseTask* m_nextPoolTask; /// ���ӵ���һ������
		virtual ~ITBaseTask() {}
		/// �̳߳ص��ȵ��������ʱִ��
		virtual void OnPoolTask() = 0;
	};

	/// ����timer�������
	struct tagOSTimerID { void* dummy; };
	typedef tagOSTimerID* TTimerID;
	typedef void(*fnTimerCallback)(void* param, TTimerID id);

#endif /* #ifdef __cplusplus */

/* �����ڴ���亯��ָ������ */
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
	// ����ͳһ��¼IP��port������ͳһʹ�á���������
	struct IPPADDR
	{
		union
		{
			struct
			{
				union
				{
					DWORD ip_; // ��������ر�˵����ip_ʹ�������ֽ���
					struct
					{
						BYTE bip0_, bip1_, bip2_, bip3_;
					};
				};
				union
				{
					INT32 port_; // ��������ر�˵����port_ʹ�ñ����ֽ���
					BYTE bport_[sizeof(INT32) / sizeof(BYTE)];
				};
			};
			INT64 i64_;
		};

		IPPADDR() : ip_(INADDR_NONE), port_(-1) {}
		IPPADDR(DWORD ip, int port) : ip_(ip), port_((INT32)port) {}

		IPPADDR(const char* ippStr, int default_port); // δʵ��
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
/* ʹ��OS_FILENAME_MATCH_CASE�Խ��ƽ̨·������Сд���ֲ��� */
#if defined(WIN32)
	#define OS_FILENAME_MATCH_CASE	false
#else
	#define OS_FILENAME_MATCH_CASE	true
#endif

/* Ĭ��trim�ַ��� */
#define STRDK_TRIM_CHARS_A		"\x20\r\n\t"
#define STRDK_TRIM_CHARS_W		L"\x20\r\n\t"
#define STRDK_TRIM_CHARS		_T("\x20\r\n\t")

/* Ĭ��·�������ַ��� */
#define PATH_FILE_INVALID_CHARS_A		"<>:|/\\\""
#define PATH_FILE_INVALID_CHARS_W		L"<>:|/\\\""
#define PATH_FILE_INVALID_CHARS			_T("<>:|/\\\"")

//·���ָ��
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
/*+++++++++++++++++++++++ ��������ʵ�� +++++++++++++++++++++++*/
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
	if (!IsSmallEndian())	//�����ֱ�ӷ���
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
	if (!IsSmallEndian())	//�����ֱ�ӷ���
		return v;
	const int l = SIZEOF(TYPE);
	TYPE r;
	BYTE* p = (BYTE*)&r;
	for (int i = 0; i < l; ++i)
		p[i] = ((BYTE*)&v)[l - 1 - i];

	return r;
}

//�ֽڰ�λ����
inline BYTE xs_byte_converse(BYTE c)
{
	c = (c & 0xaa) >> 1 | (c & 0x55) << 1;
	c = (c & 0xcc) >> 2 | (c & 0x33) << 2;
	c = (c & 0xf0) >> 4 | (c & 0x0f) << 4;
	return c;
}

// �����л����������λȡ��
// һ���÷��ǰѷ���ֵ����TYPE�ķ����෴���ͣ��� BYTE bt = SignSwitch(ch); char ch = SignSwitch(bt);
//		��������ӳ��任 BYTE[0, 255] <---> char[-128, 127]
// T_INT SIGN_SWITCH(T_INT, v);�����vΪ�����ڳ�������SIGN_SWITCH(T_INT, v)�Ǳ����ڳ�����
#define SIGN_SWITCH(T_INT, v) ((T_INT)((T_INT)(v) ^ ((T_INT)1 << (SIZEOF(T_INT)*8-1))))
template<typename T_INT>
inline T_INT SignSwitch(T_INT v)
{	return SIGN_SWITCH(T_INT, v);	}

// ����TYPE�ķ��������ж�
// bool IS_SIGNED_TYPE(TYPE);�������ڳ�����
#define IS_SIGNED_TYPE(TYPE) ((TYPE)((TYPE)0 - 1) < 0)

//-------------------------------------------------------------------------//
// ���Ϲ��������vΪָ���ʹ�÷��� p2 = (void*)AlignTo((UINT)p, (UINT)align)
#define ALIGN_TO(v, align)	(((v) + (align)-1) / (align) * (align))
template<typename TYPE, typename T2>
inline TYPE AlignTo(TYPE v, T2 align)
{	ASSERT(v >= 0 && align > 0); return ALIGN_TO(v, align);	}

// ���¹���
#define ALIGN_BACK(v, align)	((v) / (align) * (align))
template<typename TYPE, typename T2>
inline TYPE AlignBack(TYPE v, T2 align)
{	ASSERT(v >= 0 && align > 0); return ALIGN_BACK(v, align);	}

// ���϶��뵽2����������
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
	ASSERT(v2 >= v); // ��assertʧ�ܣ�˵��vֵ̫�������2����������ֵ������TYPE�޷���ʾ
	return v2;
}

//-------------------------------------------------------------------------//
// �õ�int���͵����ֵ
// T_INT GET_INT_MAX_VALUE(T_INT);�������ڳ�����
#define GET_INT_MAX_VALUE(T_INT)	(IS_SIGNED_TYPE(T_INT) ? SIGN_SWITCH(T_INT, (T_INT)-1) : (T_INT)-1)

// �õ�int���͵���Сֵ
// T_INT GET_INT_MIN_VALUE(T_INT);�������ڳ�����
#define GET_INT_MIN_VALUE(T_INT)	((T_INT)~GET_INT_MAX_VALUE(T_INT))

// �õ����͵������Сֵ
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