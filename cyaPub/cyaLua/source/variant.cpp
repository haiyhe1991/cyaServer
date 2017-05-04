#include "variant.h"

Variant::Variant()
	: m_vType(VT_NULL)
	, m_Number(0.f)
{

}

Variant::Variant(const double &value)
{
	m_Number = value;
	m_vType = VT_NUMBER;
}

Variant::Variant(const char *pStr)
{
	m_Str = pStr;
	m_vType = VT_STRING;
}

const char* Variant::GetString()
{
	return m_Str.c_str();
}

int Variant::GetInt()
{
	return int(m_Number);
}

unsigned int Variant::GetUnsignedInt()
{
	return (unsigned int)m_Number;
}

double Variant::GetDouble()
{
	return m_Number;
}

Variant::VARTYPE Variant::GetType()
{
	return m_vType;
}

const char* Variant::GetKey()
{
	return m_Key.c_str();
}

void Variant::SetNil()
{
	m_vType = VT_NULL;
	Reset();
}

void Variant::SetString(const char* pStr)
{
	m_Str = pStr;
	m_vType = VT_STRING;
}

void Variant::SetInt(int nValue)
{
	m_Number = nValue;
	m_vType = VT_NUMBER;
}

void Variant::SetUnsignedInt(unsigned int nValue)
{
	m_Number = nValue;
	m_vType = VT_NUMBER;
}

void Variant::SetDouble(double number)
{
	m_Number = number;
	m_vType = VT_NUMBER;
}

void Variant::SetType(VARTYPE type)
{
	m_vType = type;
}

void Variant::SetKey(const char* pKey)
{
	m_Key = pKey;
}

void Variant::SetTableVarVec(std::vector<Variant>& TableVarVec)
{
	m_TableVariant = TableVarVec;
	m_vType = VT_TABLE;
}

void Variant::Reset()
{
	m_Str.clear();
	m_Number = 0;
	m_vType = VT_NULL;
}


Variant& Variant::operator=(double value)
{
	m_Number = value;
	m_vType = VT_NUMBER;
	return *this;
}

Variant& Variant::operator=(const char *pStr)
{
	m_Str = pStr;
	m_vType = VT_STRING;
	return *this;
}

Variant& Variant::operator=(const Variant &va)
{
	m_Str = va.m_Str;
	m_Number = va.m_Number;
	m_vType = va.m_vType;
	return *this;
}