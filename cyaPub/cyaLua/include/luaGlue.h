// LUA脚本粘合类
// 封装C++对LUA脚本的调用

#ifndef __LUA_GLUE_H__
#define __LUA_GLUE_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

#if _MSC_VER
	#pragma warning(disable: 4121)
	#pragma warning(disable: 4315)
	#pragma warning(disable: 4996)
#endif

#include "variant.h"
#include "lua.hpp"
#include "lstate.h"

#include <vector>
#include <string>

typedef std::vector<Variant> VariantVec;

class LuaGlue
{
public:
	LuaGlue();
	LuaGlue(lua_State *pParentLuaState);
	~LuaGlue();

	lua_State* GetLuaState()
	{
		return m_pLuaState;
	}

	// 编译
	bool dofile(const char *pFileName);
	bool dostring(const char* pBuff);
	bool dobuffer(const char* pBuff, size_t sz);

	// 全局变量操作
	Variant GetGlobalVar(const char* pGlobalVarName);
	void SetGlobalVar(const char* pGlobalVarName, Variant Var);
	void GetTableAll(const char* pGlobalTableName, VariantVec& tableVecVar);
	void GetTableByKey(const char* pGlobalTableName, const char* pKey, Variant& value);
	void GetTableByIndex(const char* pGlobalTableName, int nIndex, Variant& value);

	// 执行Lua脚本函数（调用之前需按从左到右的顺序，先将函数参数Push入栈再执行）
	bool ExecFunction(const char* pFunctionName, VariantVec* pInputParamVec, int nResultNum, VariantVec& retVar);

	// 调试
	void enum_stack(lua_State *L);
	int GetStackTop();
	void CheckVersion();

private:
	bool InternalExecFunction(lua_State *L, const char* pFunctionName, VariantVec* pInputParamVec, int nResultNum, VariantVec& retVar);
	bool HandleExecResult(lua_State *L, int nResultNum, VariantVec& retVar);
	Variant GetVariantByType(int nLuaType);
	void TraverseTable(int nTableIndex, VariantVec& tableVecVar);
	void ErrorHandle();

private:
	LuaGlue(const LuaGlue&);
	LuaGlue& operator=(LuaGlue&);

private:
	lua_State *m_pLuaState;
	bool m_isOwned;
};

#endif