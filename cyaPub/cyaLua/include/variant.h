#ifndef __VARIANT_H__
#define __VARIANT_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

#if _MSC_VER
	#pragma warning(disable: 4121)
	#pragma warning(disable: 4315)
	#pragma warning(disable: 4996)
#endif

#include <string>
#include <vector>

class Variant
{
public:
	enum VARTYPE
	{
		VT_NULL,
		VT_STRING,
		VT_NUMBER,
		VT_TABLE
	};

	Variant();
	Variant(const double &value);
	Variant(const char *pStr);

public:
	const char* GetString();
	int         GetInt();
	unsigned int GetUnsignedInt();
	double      GetDouble();
	VARTYPE     GetType();
	const char* GetKey();

	void		SetNil();
	void		SetString(const char* pStr);
	void		SetInt(int nValue);
	void		SetUnsignedInt(unsigned int nValue);
	void		SetDouble(double number);
	void		SetType(VARTYPE type);
	void		SetKey(const char* pKey);
	void		SetTableVarVec(std::vector<Variant>& TableVarVec);

	void        Reset();
public:
	Variant& operator=(double value);
	Variant& operator=(const char *pStr);
	Variant& operator=(const Variant &va);

private:
	// common
	std::string m_Str;
	double m_Number;
	VARTYPE m_vType;
	std::string m_Key;

	// table	
	std::vector<Variant> m_TableVariant;
};

#endif