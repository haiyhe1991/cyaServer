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

/// �ֶ�
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

/// ��
class KeyLine : public BaseRefCount1
{
public:
	explicit KeyLine(const std::string& strKey);
	KeyLine();
	~KeyLine();

	void SetKeyName(const std::string& strKey);
	/// ��ȡKey
	const std::string& GetKeyName() const;

	/// ͨ���ֶ����ж��ֶ��Ƿ����
	bool IsExistedFiled(const std::string& filedName);
	FieldValue* GetField(const std::string& fieldName);
	/// �����ֶ�
	void AddFiled(FieldValue* theFiled);
	/// �ֶ���
	unsigned int GetFiledAmount();
	/// ��ȡ�ֶε�ֵ
	const char* GetFiledValue(const std::string& theFiledName, UINT16& valLen);
	/// �����ֶε�ֵ
	int SetFiledValue(const std::string& fileName, const char* filedVal, UINT16 valLen);
	/// ��ȡ�����ֶ�
	const std::vector<FieldValue*>& GetFileds() const;
private:
	void ClearList();
private:
	std::string m_keyName;
	typedef std::vector<FieldValue*> FiledsList;
	FiledsList m_fileds;
	CXTLocker m_listLocker;
};

/// ���
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

	/// �ֶ�����
	INT FieldType(const std::string& fieldName);

	/// ͨ��Key�ж�ĳ���Ƿ����
	bool IsExistedKey(const std::string& strKey);
	/// ͨ��Keyֵ���õ����� ,�����ڣ�����NULL
	KeyLine* GetLine(const std::string& strKey);
	/// ��ȡ���е���
	const std::map<std::string, KeyLine*>& GetLines();
	/// ͨ��Key�������ֶ�
	unsigned int AddFiled(const std::string& strKey, FieldValue* theFiled);
	/// ��������
	unsigned int AddLine(KeyLine* theKey);
	/// ɾ����
	bool DelKey(const std::string& strKey);
	/// �޸��ֶε�ֵ
	bool SetFiled(const std::string& strKey, const std::string& strFiled, const char* strVal, UINT16 valLen);
private:
	std::string m_tableName;
	typedef std::map<std::string, KeyLine*> KeyValueMap;
	KeyValueMap m_keys;			///���е���

	typedef std::map<std::string, INT> LoadFieldVec;
	LoadFieldVec m_loadFields;	/// ����Load�������ֶΣ����ڲ�ѯ

	std::string m_keyName;		/// �ֶ�PriKey������

	CXTLocker m_mapLocker;
};

/// �������ݿ�
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

	/// ����Ƿ����
	bool TalbeIsExisted(const std::string& strTableName);
	/// ���һ�ű�
	void AddTable(MemTable* theTable);
	/// ��ȡһ�ű�
	MemTable* FetchTable(const std::string& strTableName);
	/// ɾ�����ű�
	void DeleteTable(const std::string& strTableName);

	/// ��ȡ���б�
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