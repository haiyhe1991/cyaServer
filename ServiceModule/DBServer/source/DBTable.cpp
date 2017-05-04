#include "DBTable.h"
#include "cyaTcp.h"

#define EmtpyValue	""

StrFieldName::StrFieldName(const std::string& name, BOOL IsKey, INT Pos, INT types)
	: m_name(name)
	, m_iskey(IsKey)
	, m_pos(Pos)
	, m_type(types)
{

}

StrFieldName::~StrFieldName()
{

}

BOOL StrFieldName::IsKeyField() const
{
	return m_iskey;
}

INT StrFieldName::FieldPos() const
{
	return m_pos;
}

INT StrFieldName::FieldType() const
{
	return m_type;
}

const std::string& StrFieldName::FieldName() const
{
	return m_name;
}

LineFieldName::LineFieldName()
{

}

LineFieldName::~LineFieldName()
{
	CXTAutoLock locke(m_vecLocker);
	AllFiled::iterator itr = m_fileds.begin();
	for (itr; m_fileds.end() != itr; ++itr)
	{
		(*itr)->ReleaseRef();
	}
	m_fileds.clear();
}

void LineFieldName::AddFieldName(const std::string& name, BOOL IsKey, INT Pos, INT types)
{
	StrFieldName* pName = new StrFieldName(name, IsKey, Pos, types);
	if (NULL != pName)
	{
		CXTAutoLock locke(m_vecLocker);
		m_fileds.push_back(pName);
	}
}

UINT16 LineFieldName::OnFieldNum()
{
	return m_fileds.size();
}

void LineFieldName::AddFieldName(StrFieldName* pName)
{
	CXTAutoLock locke(m_vecLocker);
	m_fileds.push_back(pName);
}

BOOL LineFieldName::OnIsExsitKey()
{
	CXTAutoLock locke(m_vecLocker);
	AllFiled::iterator itr = m_fileds.begin();
	for (itr; m_fileds.end() != itr; ++itr)
	{
		if ((*itr)->IsKeyField())
		{
			return TRUE;
		}
	}
	return FALSE;
}

const std::string& LineFieldName::OnFieldName(INT Pos)
{
	CXTAutoLock locke(m_vecLocker);
	return m_fileds[Pos]->FieldName();
}

std::string InvalidFieldValue("");

const std::string& LineFieldName::OnKeyFieldNameFirst(INT& Pos)
{
	CXTAutoLock locke(m_vecLocker);
	AllFiled::iterator itr = m_fileds.begin();
	for (itr; m_fileds.end() != itr; ++itr)
	{
		if ((*itr)->IsKeyField())
		{
			Pos = (*itr)->FieldPos();
			return (*itr)->FieldName();
		}
	}
	return InvalidFieldValue;
}

INT LineFieldName::OnFieldType(INT Pos)
{
	return m_fileds[Pos]->FieldType();
}

StrValue::StrValue(const char* strVal, UINT16 valLen)
	: m_value(NULL)
	, m_valLen(valLen + 1)
{
	if (0 == valLen)
	{
		m_value = (char*)CyaTcp_Alloc(1);
		ASSERT(NULL != m_value);
		m_value[0] = '\0';
		m_valLen = 1;
	}
	if (NULL != strVal && 0 < valLen)
	{
		m_value = (char*)CyaTcp_Alloc(valLen + 1);
		if (m_value)
			memcpy(m_value, strVal, valLen);
		m_value[valLen] = '\0';
	}
}

StrValue::~StrValue()
{
	if (m_value)
	{
		CyaTcp_Free(m_value);
		m_value = NULL;
	}
	m_valLen = 0;
}

const char* StrValue::FetchValue(UINT16& valLen) const
{
	valLen = m_valLen;
	return m_value;
}

UINT16 StrValue::ValueLen() const
{
	return m_valLen;
}

TableLineValue::TableLineValue()
{

}

TableLineValue::~TableLineValue()
{
	CXTAutoLock locke(m_queLocker);
	for (UINT16 i = 0; i<m_values.size(); ++i)
	{
		m_values[i]->ReleaseRef();
	}
}

void TableLineValue::AddValue(StrValue* pValue)
{
	if (NULL != pValue)
	{
		CXTAutoLock locke(m_queLocker);
		m_values.push_back(pValue);
	}
}

UINT16 TableLineValue::FieldNum()
{
	CXTAutoLock locke(m_queLocker);
	return m_values.size();
}

UINT16 TableLineValue::LineLength()
{
	UINT16 Len = 0;
	CXTAutoLock locke(m_queLocker);
	AllValue::iterator itr = m_values.begin();
	for (itr; m_values.end() != itr; ++itr)
	{
		Len += (*itr)->ValueLen();
	}
	return Len;
}

void TableLineValue::SetKey(const std::string& keyValue)
{
	m_key = keyValue;
}

void TableLineValue::AddValue(const char* strVal, UINT16 valLen)
{
	// 	if (NULL != strVal && 0 < valLen)
	// 	{
	StrValue* p = new StrValue(strVal, valLen);
	if (NULL != p)
	{
		CXTAutoLock locke(m_queLocker);
		m_values.push_back(p);
	}
	/*	}*/
}

StrValue* TableLineValue::FetchValueByIndx(UINT16 Indx)
{
	CXTAutoLock locke(m_queLocker);
	return m_values[Indx];
}

const std::string& TableLineValue::Key() const
{
	return m_key;
}

FieldValue::FieldValue(const std::string& filed, const char* fvalue, UINT16 valLen)
	: m_filedName(filed)
	, m_filedValue(NULL)
	, m_valLen(valLen + 1)
{
	if (valLen == 0)
	{
		m_filedValue = '\0';
	}
	if (valLen > 0)
	{
		m_filedValue = (char*)CyaTcp_Alloc(valLen + 1);
		if (m_filedValue)
			memcpy(m_filedValue, fvalue, valLen);
		m_filedValue[valLen] = '\0';
	}
}

FieldValue::~FieldValue()
{
	if (m_filedValue)
		CyaTcp_Free(m_filedValue);
	m_filedValue = NULL;
	m_valLen = 0;
}

const char* FieldValue::GetFiledName() const
{
	return m_filedName.c_str();
}

const char* FieldValue::GetFiledValue(UINT16& valLen) const
{
	valLen = m_valLen;
	return m_filedValue;
}

// INT FieldValue::GetFieldType() const
// {
// 	return m_type;
// }

void FieldValue::SetFiledValue(const char* strVal, UINT16 valLen)
{
	if (m_filedValue && valLen != m_valLen)
	{
		CyaTcp_Free(m_filedValue);
		m_filedValue = (char*)CyaTcp_Alloc(valLen);
		m_valLen = valLen;
	}
	if (m_filedValue && m_valLen > 0)
		memcpy(m_filedValue, strVal, valLen);
}

KeyLine::KeyLine(const std::string& strKey)
	: m_keyName(strKey)
{

}

KeyLine::KeyLine()
{

}

KeyLine::~KeyLine()
{
	ClearList();
}

void KeyLine::SetKeyName(const std::string& strKey)
{
	m_keyName = strKey;
}

const std::string& KeyLine::GetKeyName() const
{
	return m_keyName;
}

bool KeyLine::IsExistedFiled(const std::string& fieldName)
{
	CXTAutoLock locke(m_listLocker);
	FiledsList::iterator itr = m_fileds.begin();
	for (itr; m_fileds.end() != itr; ++itr)
	{
		if ((*itr)->GetFiledName() == fieldName)
		{
			return true;
		}
	}
	return false;
}

FieldValue* KeyLine::GetField(const std::string& fieldName)
{
	CXTAutoLock locke(m_listLocker);
	FiledsList::iterator itr = m_fileds.begin();
	for (itr; m_fileds.end() != itr; ++itr)
	{
		if ((*itr)->GetFiledName() == fieldName)
		{
			return *itr;
		}
	}
	return NULL;
}

void KeyLine::AddFiled(FieldValue* theFiled)
{
	CXTAutoLock locke(m_listLocker);
	m_fileds.push_back(theFiled);
}

unsigned int KeyLine::GetFiledAmount()
{
	CXTAutoLock locke(m_listLocker);
	return m_fileds.size();
}

const char* KeyLine::GetFiledValue(const std::string& theFiledName, UINT16& valLen)
{
	CXTAutoLock locke(m_listLocker);
	FiledsList::iterator itr = m_fileds.begin();
	for (itr; m_fileds.end() != itr; ++itr)
	{
		if ((*itr)->GetFiledName() == theFiledName)
		{
			return (*itr)->GetFiledValue(valLen);
		}
	}
	return NULL;
}

int KeyLine::SetFiledValue(const std::string& filedName, const char* filedVal, UINT16 valLen)
{
	CXTAutoLock locke(m_listLocker);
	FiledsList::iterator itr = m_fileds.begin();
	for (itr; m_fileds.end() != itr; ++itr)
	{
		if ((*itr)->GetFiledName() == filedName)
		{
			(*itr)->SetFiledValue(filedVal, valLen);
			return 0;
		}
	}
	return -1;
}

const std::vector<FieldValue*>& KeyLine::GetFileds() const
{
	return m_fileds;
}

void KeyLine::ClearList()
{
	CXTAutoLock locke(m_listLocker);
	FiledsList::iterator itr = m_fileds.begin();
	for (itr; m_fileds.end() != itr; ++itr)
	{
		(*itr)->ReleaseRef();
	}
	m_fileds.clear();
}

 MemTable::MemTable(const std::string& strName)
	 : m_tableName(strName)
 {

 }

 MemTable::~MemTable()
 {

 }

 const std::string& MemTable::GetTableName() const
 {
	return m_tableName;
 }

 void MemTable::LoadAllField(const std::map<std::string, INT>& fileds, const std::string& keyName)
 {
	 CXTAutoLock locke(m_mapLocker);
	 if (m_loadFields.empty())
		m_loadFields = fileds;
	 if (m_keyName.empty())
		 m_keyName = keyName;
 }

 void MemTable::ReLoadAllField(const std::map<std::string, INT>& fileds, const std::string& keyName)
 {
	 CXTAutoLock locke(m_mapLocker);
	 m_loadFields = fileds;
	 m_keyName = keyName;
 }

 INT MemTable::FieldType(const std::string& fieldName)
 {
	 CXTAutoLock locke(m_mapLocker);
	 std::map<std::string, INT>::iterator itr = m_loadFields.find(fieldName);
	 if (m_loadFields.end() != itr)
	 {
		 return itr->second;
	 }
	 return 0;
 }

 std::string MemTable::GetAllField(std::map<std::string, INT>& fileds)
 {
	 CXTAutoLock locke(m_mapLocker);
	 fileds = m_loadFields;
	 return m_keyName;
 }

 bool MemTable::IsExistedKey(const std::string& strKey)
 {
	 CXTAutoLock locke(m_mapLocker);
	 if (m_keys.end()!=m_keys.find(strKey))
	 {
		 return true;
	 }
	 return false;
 }

 KeyLine* MemTable::GetLine(const std::string& strKey)
 {
	 CXTAutoLock locke(m_mapLocker);
	 KeyValueMap::iterator itr = m_keys.find(strKey);
	 if (m_keys.end()!=itr)
	 {
		 return itr->second;
	 }
	 return NULL;
 }

 const std::map<std::string, KeyLine*>& MemTable::GetLines()
 {
	 return m_keys;
 }

 unsigned int MemTable::AddFiled(const std::string& strKey, FieldValue* theFiled)
 {
	 KeyLine* key_val = GetLine(strKey);
	 if (NULL!=key_val)
	 {
		 key_val->AddFiled(theFiled);
		 return key_val->GetFiledAmount();
	 }
	 return 0;
 }

 unsigned int MemTable::AddLine(KeyLine* theKey)
 {
	 CXTAutoLock locke(m_mapLocker);
	 VERIFY(m_keys.insert(std::make_pair(theKey->GetKeyName(), theKey)).second);
	 return 0;
 }

 bool MemTable::DelKey(const std::string& strKey)
 {
	 CXTAutoLock locke(m_mapLocker);
	 KeyValueMap::iterator itr = m_keys.find(strKey);
	 if (m_keys.end()!=itr)
	 {
		 itr->second->ReleaseRef();
		 m_keys.erase(itr);
		 return true;
	 }
	 return false;
 }

 bool MemTable::SetFiled(const std::string& strKey, const std::string& strFiled, const char* strVal, UINT16 valLen)
 {
	 CXTAutoLock locke(m_mapLocker);
	 KeyValueMap::iterator itr = m_keys.find(strKey);
	 if (m_keys.end()!=itr)
	 {
		 itr->second->SetFiledValue(strFiled, strVal, valLen);
		 return true;
	 }
	 return false;
 }

 SQLTable* SQLTable::m_redis = NULL;
 static CXTLocker m_tableLocker;
 SQLTable::SQLTable()
 {

 }

 SQLTable::~SQLTable()
 {
	 ClearTable();
 }

 SQLTable* SQLTable::FetchTables()
 {
	 CXTAutoLock locke(m_tableLocker);
	 if (NULL == m_redis)
	 {
		 m_redis = new SQLTable();
	 }
	 return m_redis;
 }

 void SQLTable::DeleteThis()
 {
	 if (m_redis)
	 {
		 delete m_redis;
		 m_redis = NULL;
	 }
 }

 bool SQLTable::TalbeIsExisted(const std::string& strTableName)
 {
	 CXTAutoLock locke(m_tableMapLocker);
	 TableMap::iterator itr = m_tables.find(strTableName);
	 if (m_tables.end() != itr)
	 {
		 return true;
	 }
	 return false;
 }

 void SQLTable::AddTable(MemTable* theTable)
 {
	 CXTAutoLock locke(m_tableMapLocker);
	 VERIFY(m_tables.insert(std::make_pair(theTable->GetTableName(), theTable)).second);
 }

 MemTable* SQLTable::FetchTable(const std::string& strTableName)
 {
	 CXTAutoLock locke(m_tableMapLocker);
	 TableMap::iterator itr = m_tables.find(strTableName);
	 if (m_tables.end() != itr)
	 {
		 return itr->second;
	 }
	 return NULL;
 }

 void SQLTable::DeleteTable(const std::string& strTableName)
 {
	 CXTAutoLock locke(m_tableMapLocker);
	 TableMap::iterator itr = m_tables.find(strTableName);
	 if (m_tables.end() != itr)
	 {
		 itr->second->ReleaseRef();
		 m_tables.erase(itr);
	 }
 }

 const std::map<std::string, MemTable*>& SQLTable::GetAllTable()
 {
	 CXTAutoLock locke(m_tableMapLocker);
	 return m_tables;
 }

 void SQLTable::ClearTable()
 {
	 CXTAutoLock locke(m_tableMapLocker);
	 TableMap::iterator itr = m_tables.begin();
	 for (itr; m_tables.end() != itr; ++itr)
	 {
		 itr->second->ReleaseRef();
	 }
	 m_tables.clear();
 }
