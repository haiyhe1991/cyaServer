#ifndef __STR_HASH_H__
#define __STR_HASH_H__

#pragma once
#include <iostream>

class CStrHash
{
public:
	CStrHash();
	~CStrHash(void);
private:
	unsigned long cryptTable[0x500];

private:
	void InitCryptTable();                              // �Թ�ϣ������Ԥ���� 

public:
	unsigned long HashStr(const std::string& lpszString); // ��ȡ��ϣֵ   
};


#endif