#include "cyaAtomic.h"
#include "cyaRawLock.h"

#if defined(__linux__)
static inline COSRawLock& GetOSRawLock__()
{
	// 此锁在整个进程生命期都可能使用，
	// 此处构造，不析构（进程结束后锁自动释放）
	static char lockBuf[sizeof(COSRawLock)];
	static COSRawLock* lock = ::new(lockBuf)COSRawLock();
	return *lock;
}

#if defined(GCC_ATOMIC_SUPPORTED)

long InterlockedIncrement(long* val)
{
	return gcc_atomic_add(val, (long)1, true);
}

long InterlockedDecrement(long* val)
{
	return gcc_atomic_sub(val, (long)1, true);
}

long InterlockedExchange(long* val, long newVal)
{
	return gcc_atomic_swap(val, newVal);
}
//{	long v = 0; gcc_atomic_type_compare_and_swap(&v, 0, newVal);	}

#elif defined(CPU_IS_X86) && OPR_SYS_BITS == 32

long InterlockedExchange(long* val, long newVal)
{
	__asm__ __volatile__(
		"xchg %1,%0"
		: "+m" (*val), "+r" (newVal));
	return newVal;
}

long InterlockedExchangeAdd__(long* addend, long increment)
{
	__asm__ __volatile__(
		"lock; xadd %1,%0"
		: "+m" (*addend), "+r" (increment));
	return increment;
}

long InterlockedIncrement(long* val)
{
	return InterlockedExchangeAdd__(val, 1) + 1;
}

long InterlockedDecrement(long* val)
{
	return InterlockedExchangeAdd__(val, -1) - 1;
}

#else

long InterlockedIncrement(long* val)
{
	COSRawLock::AutoLock locker(GetOSRawLock__());
	return ++(*val);
}

long InterlockedDecrement(long* val)
{
	COSRawLock::AutoLock locker(GetOSRawLock__());
	return --(*val);
}

long InterlockedExchange(long* val, long newVal)
{
	long old;
	COSRawLock::AutoLock locker(GetOSRawLock__());
	old = *val;
	*val = newVal;
	return old;
}

#endif	//GCC_ATOMIC_SUPPORTED

#endif	//__linux__