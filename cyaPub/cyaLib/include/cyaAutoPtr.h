#ifndef __CYA_AUTO_PTR_H__
#define __CYA_AUTO_PTR_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

#include "cyaTypes.h"

template <typename classObj>
class RAutoPtr
{
	typedef classObj _ObjType;
private:
	RAutoPtr(const RAutoPtr&);
	RAutoPtr& operator = (const RAutoPtr&);

public:
	RAutoPtr(_ObjType* p)
		: _obj(p)
	{
		ASSERT(_obj != NULL);
	}

	~RAutoPtr()
	{
		if (_obj != NULL)
		{
			delete _obj;
			_obj = NULL;
		}
	}

	_ObjType* operator -> ()
	{
		return _obj;
	}

	_ObjType* Get()
	{
		return _obj;
	}

	operator _ObjType*&()
	{
		return _obj;
	}

	operator void*&()
	{
		return (void*&)_obj;
	}

	_ObjType* Drop()
	{
		_ObjType* p = _obj;
		_obj = NULL;
		return p;
	}

private:
	_ObjType* _obj;
};

#endif