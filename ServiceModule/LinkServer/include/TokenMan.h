#ifndef __TOKEN_MAN_H__
#define __TOKEN_MAN_H__

#include <map>
#include "cyaLocker.h"
#include "cyaTime.h"
#include "cyaCoreTimer.h"
#include "cyaRefCount.h"

#include "CyaObject.h"

class TokenMan
{
	DECLAR_FETCH_OBJ(TokenMan)
public:
	/// ���� �˻�ID�󶨵�Token
	std::string CreateToken(UINT32 accountID);
	/// �ж����˻�ID�󶨵�Token�Ƿ���Ч
	//bool TokenValid(UINT32 accountID, const std::string& accountToken);
	bool UserOnline(UINT32 accountID, const std::string& accountToken);
	void UserOffline(UINT32 accountID/*, BOOL offline*/);

	void UserFlushHeart(UINT32 accountID);

	/// �Ƿ��������
	bool Accept();
private:
	TokenMan();
	~TokenMan();

	static void TokenTimerCallback(void* param, TTimerID id);
	void OnTimerWork();
private:
	class ClientToken : public BaseRefCount1
	{
	public:
		ClientToken();
		~ClientToken();

		/// Get
		BOOL OffLine() const;
		/// Get �Ƚ�Token�Ƿ�һ��
		BOOL CompareToken(const std::string& accountToken);
		/// �Ƿ񳬹�ʧЧʱ��
		BOOL IsPastDue(DWORD pastDue);

		const BYTE* GetToken() const;
		/// ����ʱ��
		void FlushTime();
		void OffLine(BOOL fls);

		virtual void DeleteObj();
	private:
		BOOL m_offLine;
		LTMSEL m_time;		/// ����ʱ�����ˢ��ʱ��
		BYTE m_token[33];
	};
private:
	DWORD m_pastDue;	/// Token ʧЧʱ��
	CyaCoreTimer m_destroyTimer;

	typedef std::map<UINT32, ClientToken*> AccountTokenMap;
	AccountTokenMap m_tokens;
	CXTLocker m_tokenMapLocker;
};

#endif