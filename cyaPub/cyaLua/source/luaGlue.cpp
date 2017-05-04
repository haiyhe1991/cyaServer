#include "luaGlue.h"

LuaGlue::LuaGlue()
		: m_pLuaState(NULL)
		, m_isOwned(true)
{
	m_pLuaState = luaL_newstate();
	luaL_openlibs(m_pLuaState);
}

LuaGlue::LuaGlue(lua_State *pParentLuaState)
{
	m_pLuaState = lua_newthread(pParentLuaState);
	lua_pop(pParentLuaState, -1);
	m_isOwned = false;
}

LuaGlue::~LuaGlue()
{
	if (m_isOwned)
		lua_close(m_pLuaState);
}

bool LuaGlue::dofile(const char *pFileName)
{
	if (!m_pLuaState)
		return false;
	if (!pFileName)
		return false;

	if (LUA_OK != luaL_dofile(m_pLuaState, pFileName))
	{
		ErrorHandle();
		return false;
	}

	return true;
}

bool LuaGlue::dostring(const char* pBuff)
{
	if (!m_pLuaState)
		return false;
	if (!pBuff)
		return false;

	if (LUA_OK != luaL_dostring(m_pLuaState, pBuff))
	{
		ErrorHandle();
		return false;
	}

	return true;
}

bool LuaGlue::dobuffer(const char* pBuff, size_t sz)
{
	if (!m_pLuaState)
		return false;
	if (!pBuff)
		return false;

	if (LUA_OK != luaL_loadbuffer(m_pLuaState, pBuff, sz, "\"line\""))
	{
		ErrorHandle();
		return false;
	}

	return true;
}

Variant LuaGlue::GetGlobalVar(const char* pGlobalVarName)
{
	Variant Var;

	lua_getglobal(m_pLuaState, pGlobalVarName);
	switch (lua_type(m_pLuaState, -1))
	{
	case LUA_TBOOLEAN:
		Var = lua_toboolean(m_pLuaState, -1);
		break;
	case LUA_TNUMBER:
		Var = lua_tonumber(m_pLuaState, -1);
		break;
	case LUA_TSTRING:
		Var = lua_tostring(m_pLuaState, -1);
		break;
	default:
		printf("%s", lua_typename(m_pLuaState, -1));
		Var.Reset();
	}

	lua_pop(m_pLuaState, 1);
	return Var;
}

void LuaGlue::SetGlobalVar(const char* pGlobalVarName, Variant Var)
{
	switch (Var.GetType())
	{
	case Variant::VT_NUMBER:
		lua_pushnumber(m_pLuaState, Var.GetDouble());
		break;
	case Variant::VT_STRING:
		lua_pushstring(m_pLuaState, Var.GetString());
		break;
	}

	lua_setglobal(m_pLuaState, pGlobalVarName);
}

void LuaGlue::TraverseTable(int nTableIndex, VariantVec& tableVecVar)
{
	//压入nil作为table起始索引
	lua_pushnil(m_pLuaState);

	while (0 != lua_next(m_pLuaState, nTableIndex))
	{
		Variant TableVar;
		VariantVec LocalTableVarVec;
		char szKey[32] = { "" };

		// 取Key		
		switch (lua_type(m_pLuaState, -2))
		{
		case LUA_TNUMBER:
			sprintf(szKey, "%d", lua_tonumber(m_pLuaState, -2));
			TableVar.SetKey(szKey);
			break;
		case LUA_TSTRING:
			TableVar.SetKey(lua_tostring(m_pLuaState, -2));
			break;
		case LUA_TTABLE:
			printf("Table Name");
			break;
		default:
			return;
		}

		// 取Value
		switch (lua_type(m_pLuaState, -1))
		{
		case LUA_TNIL:
			TableVar.SetNil();
			break;
		case LUA_TBOOLEAN:
			TableVar.SetInt((int)lua_toboolean(m_pLuaState, -1));
			break;
		case LUA_TNUMBER:
			TableVar.SetDouble(lua_tonumber(m_pLuaState, -1));
			break;
		case LUA_TSTRING:
			TableVar.SetString(lua_tostring(m_pLuaState, -1));
			break;
		case LUA_TTABLE:
			TraverseTable(lua_gettop(m_pLuaState), LocalTableVarVec);
			TableVar.SetTableVarVec(LocalTableVarVec);
			break;
		default:
			break;
		}

		tableVecVar.push_back(TableVar);
		lua_pop(m_pLuaState, 1);
	}
}

void LuaGlue::GetTableAll(const char* pGlobalTableName, VariantVec& tableVecVar)
{
	if (!m_pLuaState)
		return;
	if (!pGlobalTableName)
		return;

	// table压栈
	lua_getglobal(m_pLuaState, pGlobalTableName);
	if (LUA_TTABLE != lua_type(m_pLuaState, -1))
	{
		ErrorHandle();
		return;
	}

	// 遍历table
	TraverseTable(lua_gettop(m_pLuaState), tableVecVar);

	// table出栈
	lua_pop(m_pLuaState, 1);
}

void LuaGlue::GetTableByKey(const char* pGlobalTableName, const char* pKey, Variant& value)
{
	if (!m_pLuaState)
		return;
	if (!pGlobalTableName)
		return;
	if (!pKey)
		return;

	lua_getglobal(m_pLuaState, pGlobalTableName);
	if (LUA_TTABLE != lua_type(m_pLuaState, -1))
	{
		ErrorHandle();
		return;
	}

	lua_pushstring(m_pLuaState, pKey);
	lua_gettable(m_pLuaState, -2);
	value = GetVariantByType(lua_type(m_pLuaState, -1));
	lua_pop(m_pLuaState, 2);
}

void LuaGlue::GetTableByIndex(const char* pGlobalTableName, int nIndex, Variant& value)
{
	if (!m_pLuaState)
		return;
	if (!pGlobalTableName)
		return;

	lua_getglobal(m_pLuaState, pGlobalTableName);
	if (LUA_TTABLE != lua_type(m_pLuaState, -1))
	{
		ErrorHandle();
		return;
	}

	lua_rawgeti(m_pLuaState, -1, nIndex);
	value = GetVariantByType(lua_type(m_pLuaState, -1));
	lua_pop(m_pLuaState, 2);
}

bool LuaGlue::ExecFunction(const char* pFunctionName, VariantVec* pInputParamVec, int nResultNum, VariantVec& retVar)
{
	if (m_pLuaState == NULL)
		return false;
	if (pFunctionName == NULL)
		return false;

	//CXTAutoLock lock(m_locker);
	int nStackTop = lua_gettop(m_pLuaState);
	bool bRet = InternalExecFunction(m_pLuaState, pFunctionName, pInputParamVec, nResultNum, retVar);
	lua_settop(m_pLuaState, nStackTop);
	return bRet;
}

bool LuaGlue::InternalExecFunction(lua_State *L, const char* pFunctionName, VariantVec* pInputParamVec, int nResultNum, VariantVec& retVar)
{
	if (lua_checkstack(L, (int)pInputParamVec->size()) == 0)
		return false;

	// Lua函数压栈
	lua_getglobal(L, pFunctionName);

	// 函数参数压栈	
	for (int i = 0; i < (int)pInputParamVec->size(); i++)
	{
		switch ((*pInputParamVec)[i].GetType())
		{
		case Variant::VT_STRING:
			lua_pushstring(L, (*pInputParamVec)[i].GetString());
			break;
		case Variant::VT_NUMBER:
			lua_pushnumber(L, (*pInputParamVec)[i].GetDouble());
			break;
		default:
			printf("执行函数%s错误,传入参数异常", pFunctionName);
			return false;
		}
	}

	// 执行函数
	if (LUA_OK != lua_pcall(L, (int)pInputParamVec->size(), nResultNum, 0))
	{
		ErrorHandle();
		return false;
	}

	HandleExecResult(L, nResultNum, retVar);
	return true;
}

bool LuaGlue::HandleExecResult(lua_State *L, int nResultNum, VariantVec& retVar)
{
	// 解析运行结果
	Variant var;
	for (int i = 0; i < nResultNum; i++)
	{
		switch (lua_type(L, -1))
		{
		case LUA_TBOOLEAN:
			var = lua_toboolean(L, -1);
			retVar.push_back(var);
			break;
		case LUA_TNUMBER:
			var = lua_tonumber(L, -1);
			retVar.push_back(var);
			break;
		case LUA_TSTRING:
			var = lua_tostring(L, -1);
			retVar.push_back(var);
			break;
		default:
			break;
			//var.Reset();
		}

		//retVar.push_back(var);
		lua_pop(L, 1);
	}

	return true;
}

Variant LuaGlue::GetVariantByType(int nLuaType)
{
	Variant Var;

	switch (nLuaType)
	{
	case LUA_TNIL:
		Var.SetNil();
		break;
	case LUA_TBOOLEAN:
		Var.SetInt((int)lua_toboolean(m_pLuaState, -1));
		break;
	case LUA_TNUMBER:
		Var.SetDouble(lua_tonumber(m_pLuaState, -1));
		break;
	case LUA_TSTRING:
		Var.SetString(lua_tostring(m_pLuaState, -1));
		break;
	case LUA_TTABLE:
		break;
	default:
		break;
	}

	return Var;
}

void LuaGlue::ErrorHandle()
{
	if (lua_isstring(m_pLuaState, -1))
		printf("dofile failed :\r\n \t%s\r\n", lua_tostring(m_pLuaState, -1));

	lua_pop(m_pLuaState, 1);
}

int LuaGlue::GetStackTop()
{
	return lua_gettop(m_pLuaState);
}

void LuaGlue::CheckVersion()
{
	luaL_checkversion(m_pLuaState/*, LUA_VERSION_NUM*/);
}

void LuaGlue::enum_stack(lua_State *L)
{
	int nTop = lua_gettop(L);
	printf("Stack Tops : %d\r\n", nTop);
	for (int i = 1; i <= nTop; ++i)
	{
		switch (lua_type(L, i))
		{
		case LUA_TNIL:
			printf("\t%s\r\n", lua_typename(L, lua_type(L, i)));
			break;
		case LUA_TBOOLEAN:
			printf("\t%s    %s\r\n", lua_typename(L, lua_type(L, i)), lua_toboolean(L, i) ? "true" : "false");
			break;
		case LUA_TLIGHTUSERDATA:
			printf("\t%s    0x%08p\r\n", lua_typename(L, lua_type(L, i)), lua_topointer(L, i));
			break;
		case LUA_TNUMBER:
			printf("\t%s    %f\r\n", lua_typename(L, lua_type(L, i)), lua_tonumber(L, i));
			break;
		case LUA_TSTRING:
			printf("\t%s    %s\r\n", lua_typename(L, lua_type(L, i)), lua_tostring(L, i));
			break;
		case LUA_TTABLE:
			printf("\t%s    0x%08p\r\n", lua_typename(L, lua_type(L, i)), lua_topointer(L, i));
			break;
		case LUA_TFUNCTION:
			printf("\t%s()  0x%08p\r\n", lua_typename(L, lua_type(L, i)), lua_topointer(L, i));
			break;
		case LUA_TUSERDATA:
			printf("\t%s    0x%08p\r\n", lua_typename(L, lua_type(L, i)), lua_topointer(L, i));
			break;
		case LUA_TTHREAD:
			printf("\t%s\r\n", lua_typename(L, lua_type(L, i)));
			break;
		}
	}
}