#ifndef __CYA_FILE_H__
#define __CYA_FILE_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

#include "cyaFdk.h"
#include "cyaStrCodec.h"

class CCyaFile
{
	FILE* fp_;
	PathName pathname_;

public:

	CCyaFile()
	{
		fp_ = NULL, pathname_[0] = 0;
	}
	~CCyaFile()
	{
		Close();
	}

	BOOL IsOpened() const
	{
		return NULL != fp_;
	}

	//²ÎÊýÍ¬fopen(...)
	BOOL Open(const char* file, const char* flag)
	{
		ASSERT(!IsOpened());
		fp_ = fopen(file, flag);
		if (fp_)
			ctotc(file, pathname_, sizeof(pathname_));
		return NULL != fp_;
	}
	BOOL Open(const WCHAR* file, const WCHAR* flag)
	{
		char szFile[MAX_PATH] = { 0 };
		char szFlag[16] = { 0 };
		return Open(wctoc(file, szFile, sizeof(szFile) - sizeof(char)), wctoc(flag, szFlag, sizeof(szFlag) - sizeof(char)));
	}

	BOOL Open(FILE* fp, const char* file)
	{
		ASSERT(!IsOpened());
		fp_ = fp;
		if (fp_)
			ctotc(file, pathname_, sizeof(pathname_));
		return NULL != fp_;
	}
	BOOL Open(FILE* fp, const WCHAR* file)
	{
		char szFile[MAX_PATH] = { 0 };
		return Open(fp, wctoc(file, szFile, sizeof(szFile) - sizeof(char)));
	}

	void Close(BOOL closeFile = true)
	{
		if (IsOpened())
		{
			if (closeFile)
				VERIFY(0 == fclose(fp_));
			fp_ = NULL;
			pathname_[0] = 0;
		}
	}

	operator FILE* () const
	{
		return fp_;
	}

	LPCTSTR FileName() const
	{
		return fp_ ? pathname_ : NULL;
	}
};


#endif