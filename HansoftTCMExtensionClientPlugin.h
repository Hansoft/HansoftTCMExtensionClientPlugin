#include "../HansoftSDK/HPMSdkCpp.h"

#include "TCMEventHandler.h"

#include <tchar.h>
#include <conio.h>
#include <windows.h>

#include <iostream>
#include <string>
#include <sstream>
#include <iterator>
#include <memory>

#define mod_export __declspec(dllexport)

using namespace std;
using namespace HPMSdk;


class CHansoftTCMExtension_ClientPlugin : public HPMSdkCallbacks
{
private:
	struct CDynamicHelper
	{
		HPMNotificationSubscription m_RightClickSubscription;
		HPMNotificationSubscription m_DynamicUpdateSubscription;
	};

	shared_ptr<CDynamicHelper> m_pDynamicHelper;
	HPMSdkSession *m_pSession;
	HPMUserContext m_UserContext;
	const HPMString s_pluginIdentifier = hpm_str("com.hansoft.tcm.clientplugin");

	std::vector<HPMUniqueID> m_LastSelectedTasks;
	HPMUniqueID m_LastSelectedProjectUID;

	std::vector<CTCMEventHandler *> m_EventHandlers;

public:



	CHansoftTCMExtension_ClientPlugin(const void *_pClientData);

	~CHansoftTCMExtension_ClientPlugin();

	virtual void On_ProcessError(EHPMError _Error);

	virtual void On_Callback(const HPMChangeCallbackData_ClientSyncDone &_Data);

	/*********************************************
	* Called when the right click menu is displayed.
	* Params:
	*	_Data - the data regarding the right click event fired (@see HPMChangeCallbackData_RightClickDisplayTaskMenu)
	*********************************************/
	virtual void On_Callback(const HPMChangeCallbackData_RightClickDisplayTaskMenu &_Data);


	/*********************************************
	* Called when a right click menu item is selected
	* Params:
	*	_Data - the data regarding the selected item (@see HPMChangeCallbackData_RightClickMenuItemSelected)
	*********************************************/
	virtual void On_Callback(const HPMChangeCallbackData_RightClickMenuItemSelected &_Data);
};

