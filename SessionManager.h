#pragma once
#include "../HansoftSDK/HPMSdkCpp.h"

using namespace std;
using namespace HPMSdk;

/// <summary>
/// Simple class that just holds the current session as a singleton for acees over the application
/// </summary>
class SessionManager
{
private:

	HPMSdkSession *m_pSession;
	SessionManager(){}
	SessionManager(const SessionManager & o){}
	static SessionManager *sessionManger;

public:

	static SessionManager &getInstance()
	{
		if (!sessionManger)
			SessionManager::sessionManger = new SessionManager();

		return *sessionManger;
	}

	/// <summary>
	/// Sets the global session object.
	/// </summary>
	void Initialize(HPMSdkSession *pSession)
	{
		m_pSession = pSession;
	}


	/// <summary>
	/// Returns the global session object.
	/// </summary>
	HPMSdkSession *Session()
	{
		return m_pSession;
	}


};

