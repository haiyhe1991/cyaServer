// 自定义内存分配器，
// 注意：本内存分配器对象，只提供静态成员形式的内存/释放函数
//		故对本内存分配器对象拷贝行为无意义。

#ifndef __CYA_ALLOCATOR_H__
#define __CYA_ALLOCATOR_H__

#if defined(_MSC_VER) && (_MSC_VER > 1000)
	#pragma once
#endif

#if defined(VS_IS_VC2008)
	#include <xmemory>
#endif

#include "cyaBase.h"

/// 基本系统内存分配器traits
struct sys_allocator_traits
{
	static inline void* Alloc(INT_PTR si)
	{
		return ::malloc(si);
	}
	static inline void Free(void* p)
	{
		::free(p);
	}

	bool operator == (const sys_allocator_traits&) const
	{
		return true;
	}
	bool operator != (const sys_allocator_traits&) const
	{
		return false;
	}
};

template<typename TYPE>
struct sys_new_delete_traits : public sys_allocator_traits
{
	typedef TYPE value_type;

	static inline TYPE* New()
	{
		return new TYPE;
	}
	static inline void Delete(TYPE* p)
	{
		delete p;
	}

	static inline TYPE* NewArray(INT_PTR count)
	{
		return new TYPE[count];
	}
	static inline void DeleteArray(TYPE* p)
	{
		delete[] p;
	}
};

// 下面的模板用于实现与sys_new_delete_traits匹配的特性类
template<typename TYPE,
	typename ALLOC /*= sys_allocator_traits*/
>
struct NewDeleteTraitsTmpl
{
	typedef TYPE value_type;

	static inline void* Alloc(INT_PTR si)
	{
		return ALLOC::Alloc(si);
	}
	static inline void Free(void* p)
	{
		ALLOC::Free(p);
	}

	static inline TYPE* New()
	{
		return ::new(ALLOC::Alloc(sizeof(TYPE))) TYPE();
	}
	static inline void Delete(TYPE* p)
	{
		p->~TYPE(); ALLOC::Free(p);
	}

	// NDTT_Type为NewDeleteTraitsTmpl的实例化类型
	// TYPE* NewEx_NDTT_(NDTT_Type, (...));
#ifdef DEBUG_NEW
	#undef DEBUG_NEW
	#define DEBUG_NEW new
#endif
#define NewEx_NDTT_(NDTT_Type, parmlist__)	\
				::new(NDTT_Type::Alloc(sizeof(NDTT_Type::value_type))) NDTT_Type::value_type parmlist__

	static inline TYPE* NewArray(INT_PTR count)
	{
		if (count <= 0 || count>1024 * 1024 * 1024)
			return NULL;
		BYTE* p = (BYTE*)ALLOC::Alloc(sizeof(INT32) + sizeof(TYPE)*count);
		if (NULL == p)
			return NULL;
		*(INT32*)p = (INT32)count;
		TYPE* r = (TYPE*)(p + sizeof(INT32));
		for (INT_PTR i = 0; i<count; ++i)
			::new(r + i) TYPE();
		return r;
	}
	static inline void DeleteArray(TYPE* p)
	{
		if (NULL == p)
			return;
		BYTE* pBase = (BYTE*)p - sizeof(INT32);
		INT32 count = *(INT32*)pBase;
		if (count <= 0)
			return;
		for (INT_PTR i = count - 1; i >= 0; --i)
			(p + i)->~TYPE();
		ALLOC::Free(pBase);
	}
};

/* C++自定义内存分配器模板代码
struct xxx_AllocTraits
{
static inline void* Alloc(INT_PTR si)
{	return xxx_Alloc(si);	}
static inline void Free(void* p)
{	xxx_Free(p);	}

bool operator== (const xxx_AllocTraits&) const
{	return true;	}
bool operator!= (const xxx_AllocTraits&) const
{	return false;	}
};

#define xxx_New(TYPE)					\
::new(xxx_Alloc(sizeof(TYPE))) TYPE()

#define xxx_NewEx(TYPE, parmlist__)		\
::new(xxx_Alloc(sizeof(TYPE))) TYPE parmlist__

template<typename TYPE>
inline void xxx_Delete(TYPE* p)
{	p->~TYPE(); xxx_Free(p);	}

template<typename TYPE>
struct xxx_NewDeleteTraits : public NewDeleteTraitsTmpl<TYPE, xxx_AllocTraits>
{
};

#define xxx_NewArray(TYPE, count)		\
xxx_NewDeleteTraits<TYPE>::NewArray(count)

template<typename TYPE>
inline void xxx_DeleteArray(TYPE* p)
{	xxx_NewDeleteTraits<TYPE>::DeleteArray(p);	}
*/


// 供stl使用的内存分配器traits模板
template<typename TYPE,
	typename BASE_ALLOC /*= sys_allocator_traits*/
>
class xstl_allocator
#if defined(_MSC_VER) && (_MSC_VER >= 1500) // >=VC2008
	: public std::_Allocator_base<TYPE>
#endif
{
private:
	BASE_ALLOC baseAlloc_;

public:
	const BASE_ALLOC& GetBaseAlloc() const
	{
		return baseAlloc_;
	}

public:
#if defined(_MSC_VER) && (_MSC_VER >= 1500) // >=VC2008
	typedef std::_Allocator_base<TYPE> BaseAllocator;
	typedef typename BaseAllocator::value_type value_type;
#else
	typedef TYPE value_type;
#endif
	typedef value_type* pointer;
	typedef value_type& reference;
	typedef const value_type* const_pointer;
	typedef const value_type& const_reference;
	typedef size_t size_type;
	typedef INT_PTR difference_type;

	xstl_allocator() throw()
	{}
	~xstl_allocator()
	{}
	xstl_allocator(const xstl_allocator&) throw()
	{}
	xstl_allocator& operator= (const xstl_allocator&)
	{
		return *this;
	}

#if defined(VS_IS_VC6)
	char* _Charalloc(size_type n)
	{
		return (char*)baseAlloc_.Alloc(n);
	}
#else
	template<typename OTHER>
	struct rebind
	{
		typedef xstl_allocator<OTHER, BASE_ALLOC> other;
	};

	template<typename OTHER>
	xstl_allocator(const xstl_allocator<OTHER, BASE_ALLOC>&) throw()
	{}
	template<typename OTHER>
	xstl_allocator& operator= (const xstl_allocator<OTHER, BASE_ALLOC>&)
	{
		return *this;
	}

	pointer address(reference r) const
	{
		return &r;
	}
	const_pointer address(const_reference r) const
	{
		return &r;
	}
#endif

	pointer allocate(size_type n)
	{
		return (pointer)baseAlloc_.Alloc(sizeof(TYPE) * n);
	}
	pointer allocate(size_type n, const void*)
	{
		return allocate(n);
	}
	void deallocate(void* p, size_type)
	{
		baseAlloc_.Free(p);
	}

#if defined(_MSC_VER)
	void construct(pointer p, const TYPE& r)
	{
		std::_Construct(p, r);
	}
	void destroy(pointer p)
	{
		std::_Destroy(p);
	}
#else
	void construct(pointer p, const TYPE& r)
	{
		::new(p)TYPE(r);
	}
	void destroy(pointer p)
	{
		p->~TYPE();
	}
#endif

	size_t max_size() const throw()
	{
		size_t n = (size_t)(-1) / sizeof(TYPE);
		return (0<n ? n : 1);
	}
};

template<typename TYPE1, typename TYPE2, typename BASE_ALLOC>
inline bool operator == (const xstl_allocator<TYPE1, BASE_ALLOC>& l, const xstl_allocator<TYPE2, BASE_ALLOC>& r) throw()
{
	return l.GetBaseAlloc() == r.GetBaseAlloc();
}

template<typename TYPE1, typename TYPE2, typename BASE_ALLOC>
inline bool operator != (const xstl_allocator<TYPE1, BASE_ALLOC>& l, const xstl_allocator<TYPE2, BASE_ALLOC>& r) throw()
{
	return l.GetBaseAlloc() != r.GetBaseAlloc();
}

#endif