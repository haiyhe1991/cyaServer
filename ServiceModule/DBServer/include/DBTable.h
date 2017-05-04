#ifndef __DB_TABLE_H__
#define __DB_TABLE_H__

#include <string>
#include <map>
#include <list>
#include <vector>
#include <queue>

#include "cyaTypes.h"
#include "cyaRefCount.h"
#include "cyaLocker.h"

class StrFieldName : public BaseRefCount1
{
public:
	StrFieldName(const std::string& name, BOOL IsKey, INT Pos, INT types);
	~StrFieldName();

	BOOL IsKeyField() const;
	INT FieldPos() const;
	INT FieldType() const;
	const std::string& FieldName() const;
private:
	std::string m_name;
	BOOL m_iskey;
	INT m_pos;
	INT m_type;
};

class LineFieldName : public BaseRefCount1
{
public:
	LineFieldName();
	~LineFieldName();

	void AddFieldName(const std::string& name, BOOL IsKey, INT Pos, INT types);
	void AddFieldName(StrFieldName* pName);

	UINT16 OnFieldNum();

	BOOL OnIsExsitKey();
	const std::string& OnFieldName(INT Pos);
	const std::string& OnKeyFieldNameFirst(INT& Pos);
	INT OnFieldType(INT Pos);
private:
	typedef std::vector<StrFieldName*> AllFiled;
	AllFiled m_fileds;
	CXTLocker m_vecLocker;
};

class StrValue : public BaseRefCount1
{
public:
	StrValue(const char* strVal, UINT16 valLen);
	~StrValue();

	const char* FetchValue(UINT16& valLen) const;
	UINT16 ValueLen() const;
private:
	char* m_value;
	UINT16 m_valLen;
};

class TableLineValue : public BaseRefCount1
{
public:
	TableLineValue();
	~TableLineValue();

	void AddValue(StrValue* pValue);
	void AddValue(const char* strVal, UINT16 valLen);
	StrValue* FetchValueByIndx(UINT16 Indx);
	const std::string& Key() const;
	UINT16 FieldNum();
	UINT16 LineLength();
	void SetKey(const std::string& keyValue);
private:
	typedef std::vector<StrValue*> AllValue;
	AllValue m_values;
	CXTLocker m_queLocker;
	std::string m_key;
};

/// 字段
class FieldValue : public BaseRefCount1
{
public:
	FieldValue(const std::string& filed, const char* fvalue, UINT16 valLen);
	~FieldValue();

	const char* GetFiledName() const;
	const char* GetFiledValue(UINT16& valLen) const;
	//	INT GetFieldType() const;
	void SetFiledValue(const char* strVal, UINT16 valLen);
private:
	std::string m_filedName;
	char* m_filedValue;
	UINT16 m_valLen;
};

/// 行
class KeyLine : public BaseRefCount1
{
public:
	explicit KeyLine(const std::string& strKey);
	KeyLine();
	~KeyLine();

	void SetKeyName(const std::string& strKey);
	/// 获取Key
	const std::string& GetKeyName() const;

	/// 通过字段名判断字段是否存在
	bool IsExistedFiled(const std::string& filedName);
	FieldValue* GetField(const std::string& fieldName);
	/// 插入字段
	void AddFiled(FieldValue* theFiled);
	/// 字段数
	unsigned int GetFiledAmount();
	/// 获取字段的值
	const char* GetFiledValue(const std::string& theFiledName, UINT16& valLen);
	/// 设置字段的值
	int SetFiledValue(const std::string& fileName, const char* filedVal, UINT16 valLen);
	/// 获取所有字段
	const std::vector<FieldValue*>& GetFileds() const;
private:
	void ClearList();
private:
	std::string m_keyName;
	typedef std::vector<FieldValue*> FiledsList;
	FiledsList m_fileds;
	CXTLocker m_listLocker;
};

/// 表格
class MemTable : public BaseRefCount1
{
public:
	explicit MemTable(const std::string& strTable);
	~MemTable();

	const std::string& GetTableName() const;

	/// SQL Table Load To Memmery Table, Load Filed And KeyName
	void LoadAllField(const std::map<std::string, INT>& fileds, const std::string& keyName);
	void ReLoadAllField(const std::map<std::string, INT>& fileds, const std::string& keyName);
	std::string GetAllField(std::map<std::string, INT>& fileds);

	/// 字段类型
	INT FieldType(const std::string& fieldName);

	/// 通过Key判定某行是否存在
	bool IsExistedKey(const std::string& strKey);
	/// 通过Key值，得到整行 ,不存在：返回NULL
	KeyLine* GetLine(const std::string& strKey);
	/// 获取所有的行
	const std::map<std::string, KeyLine*>& GetLines();
	/// 通过Key，插入字段
	unsigned int AddFiled(const std::string& strKey, FieldValue* theFiled);
	/// 插入整行
	unsigned int AddLine(KeyLine* theKey);
	/// 删除行
	bool DelKey(const std::string& strKey);
	/// 修改字段的值
	bool SetFiled(const std::string& strKey, const std::string& strFiled, const char* strVal, UINT16 valLen);
private:
	std::string m_tableName;
	typedef std::map<std::string, KeyLine*> KeyValueMap;
	KeyValueMap m_keys;			///所有的行

	typedef std::map<std::string, INT> LoadFieldVec;
	LoadFieldVec m_loadFields;	/// 所有Load进来的字段，便于查询

	std::string m_keyName;		/// 字段PriKey的名字

	CXTLocker m_mapLocker;
};

/// 整个数据库
class SQLTable
{
protected:
	SQLTable();
	~SQLTable();
private:
	SQLTable& operator=(const SQLTable&);
	SQLTable(const SQLTable&);
public:
	static SQLTable* FetchTables();
	void DeleteThis();

	/// 表格是否存在
	bool TalbeIsExisted(const std::string& strTableName);
	/// 添加一张表
	void AddTable(MemTable* theTable);
	/// 获取一张表
	MemTable* FetchTable(const std::string& strTableName);
	/// 删除整张表
	void DeleteTable(const std::string& strTableName);

	/// 获取所有表
	const std::map<std::string, MemTable*>& GetAllTable();
private:
	void ClearTable();
private:
	static SQLTable* m_redis;
	typedef std::map<std::string, MemTable*> TableMap;
	TableMap m_tables;
	CXTLocker m_tableMapLocker;
};

#endif