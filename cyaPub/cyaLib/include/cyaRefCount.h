#ifndef __CYA_REF_COUNT_H__
#define __CYA_REF_COUNT_H__

#if defined(_MSC_VER) &&_MSC_VER > 1000
	#pragma once
#endif

#include "cyaAtomic.h"

struct IBaseRefCount
{
private:
	IBaseRefCount(const IBaseRefCount&);
	IBaseRefCount& operator= (const IBaseRefCount&);
public:
	IBaseRefCount(){}

public:
	virtual long AddRef() = 0;
	virtual long ReleaseRef() = 0;

protected:
	virtual ~IBaseRefCount() {} // Must be virtual
};

// 引用计数基类，建议init_refCount总是用1
template<typename Base, long init_refCount /*= 1*/>
class BaseRefCountTmpl : public Base
{
	BaseRefCountTmpl(const BaseRefCountTmpl&);
	BaseRefCountTmpl& operator= (const BaseRefCountTmpl&);

	long m_refCount;

protected:
	BaseRefCountTmpl() : m_refCount(init_refCount) {}
	virtual ~BaseRefCountTmpl() {} // Must be virtual

public:
	virtual long GetRefCount() const
	{
		return m_refCount;
	}

	virtual void DeleteObj()
	{
		delete this;
	}

	virtual long AddRef() // 返回加计数后的结果
	{
		ASSERT(m_refCount >= init_refCount);
		long n = InterlockedIncrement(&m_refCount);
		ASSERT(m_refCount >= 1);
		return n;
	}

	virtual long ReleaseRef() // 返回减计数后的结果
	{
		ASSERT(m_refCount > 0);
		long n = InterlockedDecrement(&m_refCount);
		ASSERT(n >= 0); // 防止Release()次数过多
		if (n == 0)
			DeleteObj();
		return n;
	}
};

typedef BaseRefCountTmpl<IBaseRefCount, 0> BaseRefCount0;
typedef BaseRefCountTmpl<IBaseRefCount, 1> BaseRefCount1;

template <typename classObj>
class AutoRefCountObjTmpl
{
	typedef classObj _classType;
	AutoRefCountObjTmpl(const AutoRefCountObjTmpl&);
	AutoRefCountObjTmpl& operator = (const AutoRefCountObjTmpl&);

public:
	explicit AutoRefCountObjTmpl(_classType* obj, BOOL bAddRef = false) : _obj(obj)
	{
		ASSERT(_obj != NULL);
		if (bAddRef && _obj != NULL)
			_obj->AddRef();
	}

	~AutoRefCountObjTmpl()
	{
		if (_obj != NULL)
		{
			_obj->ReleaseRef();
			_obj = NULL;
		}
	}

	_classType* operator -> ()
	{
		return _obj;
	}

	operator _classType*&()
	{
		return _obj;
	}

	operator void*&()
	{
		return (void*&)_obj;
	}

	_classType* Get()
	{
		return _obj;
	}

	_classType* Detach()
	{
		_classType* p = _obj;
		_obj = NULL;
		return p;
	}

	void Attach(_classType* p)
	{
		if (_obj)
			_obj->ReleaseRef();
		_obj = p;
	}

private:
	_classType* _obj;
};

#endif