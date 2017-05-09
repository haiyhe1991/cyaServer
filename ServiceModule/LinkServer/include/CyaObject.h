/*
一些简单的单例等获取、销毁对象宏
*/
#ifndef __CYA_OBJECT_H__
#define __CYA_OBJECT_H__

#include "cyaTypes.h"

#define  DECLAR_FETCH_OBJ(className) \
	public:	\
	static className* Fetch##className(); \
	void DeleteThis(); \
	private:	\
	static className* m_this; \
	className(const className&); \
	className& operator=(const className&);

#define DEFINE_FETCH_OBJ(className) \
	className* className::m_this = NULL; \
	className* className::Fetch##className() { \
		if (NULL == m_this) \
			m_this = new className(); \
		return m_this;	\
	} \
	void className::DeleteThis() { \
		if (NULL != m_this) { \
			delete m_this; \
			m_this = NULL; \
		} \
	}

#define DELAR_GLOBAL_OBJ(className) \
	static className* g_obj##className = NULL;

#define NEW_FETCH_OBJ(className) \
	if (NULL == g_obj##className) \
		g_obj##className = new className();

#define GET_GLOBAL_OBJ(className) g_obj##className

#define DELETE_FETCH_OBJ(className) \
	delete g_obj##className; \
	g_obj##className = NULL;

#define DELETE_FETCH_OBJ_EX(className) \
	if (NULL != g_obj##className) { \
		delete g_obj##className; \
		g_obj##className = NULL; \
	}

#define VALID_GLOBAL_OBJ(className) \
	if (NULL == g_obj##className)	return 1;

#define NEW_FETCH_OBJ_EX(className)\
	NEW_FETCH_OBJ(className) \
	VALID_GLOBAL_OBJ(className)

#define DELETE_PRIVATE_OBJ(objName) \
	if (NULL != objName) { \
		delete objName;	\
		objName = NULL;	\
	}

#endif