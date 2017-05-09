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
	/// 返回 账户ID绑定的Token
	std::string CreateToken(UINT32 accountID);
	/// 判断与账户ID绑定的Token是否有效
	//bool TokenValid(UINT32 accountID, const std::string& accountToken);
	bool UserOnline(UINT32 accountID, const std::string& accountToken);
	void UserOffline(UINT32 accountID/*, BOOL offline*/);

	void UserFlushHeart(UINT32 accountID);

	/// 是否接受连接
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
		/// Get 比较Token是否一致
		BOOL CompareToken(const std::string& accountToken);
		/// 是否超过失效时间
		BOOL IsPastDue(DWORD pastDue);

		const BYTE* GetToken() const;
		/// 更新时间
		void FlushTime();
		void OffLine(BOOL fls);

		virtual void DeleteObj();
	private:
		BOOL m_offLine;
		LTMSEL m_time;		/// 创建时间或者刷新时间
		BYTE m_token[33];
	};
private:
	DWORD m_pastDue;	/// Token 失效时间
	CyaCoreTimer m_destroyTimer;

	typedef std::map<UINT32, ClientToken*> AccountTokenMap;
	AccountTokenMap m_tokens;
	CXTLocker m_tokenMapLocker;
};

#endif