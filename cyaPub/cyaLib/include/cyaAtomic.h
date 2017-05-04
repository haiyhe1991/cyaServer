#ifndef __CYA_ATOMIC_H__
#define __CYA_ATOMIC_H__

#include "cyaTypes.h"
#include "cyaTime.h"

#if defined(__linux__)
	long InterlockedExchange(long* val, long newVal);
	long InterlockedIncrement(long* val);
	long InterlockedDecrement(long* val);
#endif

#if defined(GCC_ATOMIC_SUPPORTED)
	#include <pthread.h>
	template <typename type>
	inline type gcc_atomic_add(type* val, type step, bool retVal = true)
	{
		if (retVal)
			return __sync_add_and_fetch(val, step);
		else
			return __sync_fetch_and_add(val, step);
	}

	template <typename type>
	inline type gcc_atomic_sub(type* val, type step, bool retVal = true)
	{
		if (retVal)
			return __sync_sub_and_fetch(val, step);
		else
			return __sync_fetch_and_sub(val, step);
	}

	template <typename type>
	inline type gcc_atomic_or(type* val, type step, bool retVal = true)
	{
		if (retVal)
			return __sync_or_and_fetch(val, step);
		else
			return __sync_fetch_and_or(val, step);
	}

	template <typename type>
	inline type gcc_atomic_and(type* val, type step, bool retVal = true)
	{
		if (retVal)
			return __sync_and_and_fetch(val, step);
		else
			return __sync_fetch_and_and(val, step);
	}

	template <typename type>
	inline type gcc_atomic_xor(type* val, type step, bool retVal = true)
	{
		if (retVal)
			return __sync_xor_and_fetch(val, step);
		else
			return __sync_fetch_and_xor(val, step);
	}

	template <typename type>
	inline type gcc_atomic_nand(type* val, type step, bool retVal = true)
	{
		if (retVal)
			return __sync_nand_and_fetch(val, step);
		else
			return __sync_fetch_and_nand(val, step);
	}

	template <typename type>
	inline bool gcc_atomic_bool_compare_and_swap(type* val, type oldVal, type newVal)
	{
		return __sync_bool_compare_and_swap(val, oldVal, newVal);
	}

	template <typename type>
	inline type gcc_atomic_type_compare_and_swap(type* val, type oldVal, type newVal)
	{
		return __sync_val_compare_and_swap(val, oldVal, newVal);
	}

	template <typename type>
	inline type gcc_atomic_swap(type* val, type newVal)
	{
		return __sync_lock_test_and_set(val, newVal);
	}
#endif

/* ×ÔÐý¼òµ¥Ëø */
void OSSpinLockInit(LONG* lock);
void OSSpinLock(LONG* lock, DWORD spinMsel DEFAULT_PARAM(0));
void OSSpinUnlock(LONG* lock);
BOOL OSIsSpinLocked(LONG* lock);

inline void OSSpinLockInit(LONG* lock)
{
	ASSERT(lock); *lock = 0;
}

inline void OSSpinLock(LONG* lock, DWORD spinMsel /*= 0*/)
{
	ASSERT(lock);
	while (InterlockedExchange(lock, 1))
	{
		Sleep(spinMsel);
	}
}

inline void OSSpinUnlock(LONG* lock)
{
	ASSERT(lock);	InterlockedExchange(lock, 0);
}

inline BOOL OSIsSpinLocked(LONG* lock)
{
	ASSERT(lock);	return !!InterlockedExchange(lock, *lock);
}
#endif