#ifndef __DELEGET_MSG_H__
#define __DELEGET_MSG_H__

#include <map>
#include "cyaTypes.h"
#include "cyaLocker.h"
#include "ServiceProtocol.h"

class DelegetMessage
{
private:
	struct USER_INFO
	{
		void* m_session;
		UINT32 m_cookie;
		char m_key[DEFAULT_KEY_SIZE + 1];
	};
public:
	DelegetMessage();
	~DelegetMessage();

	static DelegetMessage* InstanceDeleteget();
	static void DeleteThis();

	INT DelegetMsgHandle(const void* msgDaga, INT msgLen, void* theSession, UINT32 userId);
	const char* FindEncryptKey(UINT32 cookie);
	void* FindSession(UINT32 cookie);
	void UpdateSessionLot(UINT32 cookie, void* theSession);
	INT ResponseErrMsg(UINT32 token, UINT16 cmdCode, UINT16 retCode, void* theSession, const char* key);
	void LoginFailedHandle(UINT32 cookie);
private:
	BOOL ValidSession(UINT32 cookie);
	/// 判定账户的有效性，token>0不有效token
	BOOL VaildAccount(const char* account, UINT32& token);
	INT  Login(UINT32 token, void* theSession, USER_INFO*& info);
	INT  Logout(UINT32 cookie, char* key);
	UINT32 GetSessionCookie(const void* msgData, INT msgLen);
	BOOL AddUserInfo(UINT32 token, USER_INFO* info);

	INT ResponseDataMsg(void* theSession, const void*msgData, INT msgLen, UINT16 cmdCode, UINT32 token, const char* key);
private:
	typedef std::map<UINT32, USER_INFO*> UserCookieMap;	/// <Token, Session>

	static DelegetMessage* m_this;
	UserCookieMap m_userCookies;
	CXTLocker m_mapLocker;
};


#endif