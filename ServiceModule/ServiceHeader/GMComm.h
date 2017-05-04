#ifndef __GM_COMM_H__
#define __GM_COMM_H__

#include "cyaTcp.h"
#include "cyaBase.h"
#include "ServiceProtocol.h"
#include "GMProtocal.h"

#define GET_SGS_PROTOHEAD_SIZE		sizeof(SGSProtocolHead)
#define GET_GM_PROTOHEAD_SIZE		sizeof(GMProtocalHead)
//��ȡ�������ݽṹ���С;
#define GET_SGSPAYLOAD_REQ_SIZE	(sizeof(SGSReqPayload) - sizeof(char))
//��ȡ��Ӧ���ݽṹ���С;
#define GET_SGSPAYLOAD_RES_SIZE	(sizeof(SGSResPayload) - sizeof(char))
#define MAX_LIMIT_COLUMS			500 //������Ƽ�¼��

///��ȡ�ṹ���С
#define GET_STRUC_SIZE(struc) sizeof(struc) 


///��ȡ�ṹ���С
#define GET_ANY_STRUC_SIZE(struc, tp) (tp==0?(sizeof(struc)): (sizeof(struc) - sizeof(char)))


///��ȡ������
#define  GET_COMMAND(data, struc) \
	ntohs(*(UINT16*)(BYTE*)data + member_offset(struc, rawDataBytes, cmdCode));

///����Э��ͷ����
///@head[OUT]: Э��ͷ
///@len[IN]: ���ݳ���
///@pkType[IN]: ������
///@isCrypt[IN]: �Ƿ����
#define SGS_PROTO_HEAD_DATA_MAN__(head, len, pkType, iscpt) \
	head->pduLen = len; \
	head->pktType = pkType; \
	head->isCrypt = iscpt; \
	head->hton();

///GM����Э��ͷ����
///@head[OUT]: Э��ͷ
///@len[IN]: ���ݳ���
///@pkType[IN]: ������
///@isCrypt[IN]: �Ƿ����
///@isOver[IN]: �Ƿ�������
#define GM_PROTO_HEAD_DATA_MAN__(head, len, pkType, iscpt, sreq, iover) \
	head->pduLen = len; \
	head->pktType = pkType; \
	head->isCrypt = iscpt; \
	head->req = sreq; \
	head->isOver = iover; \
	head->hton();

///GM����Э��ͷ����
///@head[OUT]: Э��ͷ
///@len[IN]: ���ݳ���
///@pkType[IN]: ������
///@isCrypt[IN]: �Ƿ����
///@isOver[IN]: �Ƿ�������
#define GM_PROTO_HEAD_DATA_MAN1__(head, len, pkType, iscpt, sreq, iover) \
	head.pduLen = len; \
	head.pktType = pkType; \
	head.isCrypt = iscpt; \
	head.req = sreq; \
	head.isOver = iover; \
	head.hton();

///����Э��������(����)
///@qpayload[OUT]: ����Э����
///@qrawlen[IN]: ԭʼ���ݳ���
///@qtoken[IN]: ����
///@qcmd[IN]: ������
#define SGS_PROTO_PAYLOAD_REQ_DATA_MAN__(qpayload, qrawlen, qtoken, qcmd) \
	qpayload->rawDataBytes = qrawlen; \
	qpayload->token = qtoken; \
	qpayload->cmdCode = qcmd; \
	qpayload->hton();


///����Э��������(����)
///@qpayload[OUT]: ����Э����
///@qrawlen[IN]: ԭʼ���ݳ���
///@qtoken[IN]: ����
///@qcmd[IN]: ������
#define SGS_PROTO_PAYLOAD_RES_DATA_MAN__(rpayload, rrawlen, rtoken, rcmd, rretcode) \
	rpayload->rawDataBytes = rrawlen; \
	rpayload->token = rtoken; \
	rpayload->cmdCode = rcmd; \
	rpayload->retCode = rretcode; \
	rpayload->hton();



///�������ݳ���16�ֽڶ���
inline UINT16 BytesAlign(UINT16& sourceLen)
{
	return sourceLen % 16 == 0 ? sourceLen : (((sourceLen >> 4) + 1) << 4);
}

///�ڴ���䴦��
inline void* DataAlloc(int nSize)
{
	return CyaTcp_Alloc(nSize + 1);
}


///��ȡϵͳ��ǰʱ��
inline void GetSysCurrentTime(char stime[20])
{
	SYSTEMTIME st;
	memset(stime, 0, 20);
	ToTimeEx(GetMsel(), &st, TRUE);
	sprintf_traits(stime, _T("%04d-%02d-%02d %02d:%02d:%02d"), st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);

}

///��[2014-02-01 11:11:12]תΪLTMSEL
inline LTMSEL StrToMsel1(const TCHAR* pszDate, BOOL toLocal /*= true*/)
{
	SYSTEMTIME st;
	memset(&st, 0, sizeof(SYSTEMTIME));
	int nYear = 0, nMonth = 0, nDay = 0, nHour = 0, nMinute = 0, nSecond = 0;
	_stscanf(pszDate, _T("%04d-%02d-%02d %02d:%02d:%02d"), &nYear, &nMonth, &nDay, &nHour, &nMinute, &nSecond);
	st.wYear = (WORD)nYear;
	st.wMonth = (WORD)nMonth;
	st.wDay = (WORD)nDay;
	st.wHour = (WORD)nHour;
	st.wMinute = (WORD)nMinute;
	st.wSecond = (WORD)nSecond;
	return ToMsel(st, toLocal);
}

///��[20140201111112]תΪLTMSEL
inline LTMSEL StrToMsel2(const TCHAR* pszDate, BOOL toLocal /*= true*/)
{
	SYSTEMTIME st;
	memset(&st, 0, sizeof(SYSTEMTIME));
	int nYear = 0, nMonth = 0, nDay = 0, nHour = 0, nMinute = 0, nSecond = 0;
	_stscanf(pszDate, _T("%04d%02d%02d%02d%02d%02d"), &nYear, &nMonth, &nDay, &nHour, &nMinute, &nSecond);
	st.wYear = (WORD)nYear;
	st.wMonth = (WORD)nMonth;
	st.wDay = (WORD)nDay;
	st.wHour = (WORD)nHour;
	st.wMinute = (WORD)nMinute;
	st.wSecond = (WORD)nSecond;
	return ToMsel(st, toLocal);
}

///��ȡʱ��(isNow= TRUE:��ȡ��ǰϵͳʱ��, isNow= FALSE:��ȡstime�е�ʱ��)
inline void GetTime(const char* stime, char buf[20], BOOL isNow)
{
	SYSTEMTIME sysTime;
	if (isNow)
		sysTime = ToTime(GetMsel(), TRUE);
	else
		sysTime = ToTime(StrToMsel1(stime, TRUE), TRUE);
	memset(buf, 0, strlen(buf));
	sprintf(buf, "%02d:%02d:%02d", sysTime.wHour, sysTime.wMinute, sysTime.wSecond);
}

///��ȡ����(isNow= TRUE:��ȡ��ǰϵͳ����, isNow= FALSE:��ȡstime�е�����)
inline void GetDate(const char* stime, char buf[20], BOOL isNow)
{
	SYSTEMTIME sysTime;
	if (isNow)
		sysTime = ToTime(GetMsel(), TRUE);
	else
		sysTime = ToTime(StrToMsel1(stime, TRUE), TRUE);
	memset(buf, 0, strlen(buf));
	sprintf(buf, "%4d-%02d-%02d", sysTime.wYear, sysTime.wMonth, sysTime.wDay);
}

inline void StrToDate(const char *str, char stime[20])
{
	int year;
	int month;
	int day;
	sscanf(str, "%04d-%02d-%02d", &year, &month, &day);
	sprintf(stime, "%d-%d-%d", year, month, day);
}

///��ȡĿ¼������ͬ���͵��ļ�
//inline bool IterateSameTypeFile(const char* filePath, const char* suffixName, std::vector<std::string/*�����ļ�·��*/>& fvec)
//{
//#if defined (WIN32)
//	char szKeyFilePath[MAX_PATH] = { 0 };
//	strcpy(szKeyFilePath, filePath);
//	strcat(szKeyFilePath, "\\*.");
//	strcat(szKeyFilePath, suffixName);
//	WIN32_FIND_DATAA fd;
//	HANDLE hd = FindFirstFileA(szKeyFilePath, &fd);
//	if(hd == INVALID_HANDLE_VALUE)
//		return false;
//
//	do 
//	{
//		char szfileFullPath[MAX_PATH] = { 0 };
//		strcpy(szfileFullPath, filePath);
//		strcat(szfileFullPath, "\\");
//		strcat(szfileFullPath, fd.cFileName);
//		fvec.push_back(szfileFullPath);
//	} while (FindNextFileA(hd, &fd));
//	FindClose(hd);
//	return true;
//#else
//	SCAN_FILE_FILTER scanfilter;
//	scanfilter.count = scandir(filePath, &scanfilter.nameList, SCAN_FILE_FILTER::scandir_filter_, alphasort);
//	int count = scanfilter.count;
//	struct dirent** nameList = scanfilter.nameList;
//	if(count <= 0 || nameList == NULL)
//		return false;
//	for (int i = 0; i < count; i++)
//	{
//		char szfileFullPath[MAX_PATH] = { 0 };
//		strcpy(szfileFullPath, filePath);
//		strcat(szfileFullPath, "/");
//		strcat(szfileFullPath, nameList[i]->d_name);
//
//		const char* fileSuffixName = GetFileSuffixName(szfileFullPath);
//		if(strcmp(fileSuffixName, suffixName) != 0)
//			continue;
//
//		fvec.push_back(szfileFullPath);
//	}
//	return true
//#endif
//}


///��ȡǰ׺��
inline void GetFilePrefixName(char fpath[128], int len, char prefixName[128])
{
	char* pfPath = fpath;
	for (int i = 0; i < len; i++)
	{
		if (pfPath[i] != '.')
			prefixName[i] = pfPath[i];
		else
			break;
	}
}


inline void FreeAlloc(void* p)
{
	if (p != NULL)
		CyaTcp_Free(p);

}


//Check data is NULL string
inline bool StrIsNull(const char* p)
{
	if (p == NULL || strlen(p) == 0)
		return true;
	return false;
}

#endif