#include "HansoftTCMExtensionClientPlugin.h"
#include "TCMTestCycleEventHandler.h"
#include "TCMRequirementsEventHandler.h"
#include "SessionManager.h"


CHansoftTCMExtension_ClientPlugin *g_pClientPlugin;

extern "C" mod_export void DHPMSdkCallingConvention HPMClientSDKCreate(const void *_pClientData)
{
	g_pClientPlugin = new CHansoftTCMExtension_ClientPlugin(_pClientData);
}

extern "C" mod_export void DHPMSdkCallingConvention HPMClientSDKDestroy()
{
	delete g_pClientPlugin;
}

CHansoftTCMExtension_ClientPlugin::CHansoftTCMExtension_ClientPlugin(const void *_pClientData)
{

	try
	{
		m_UserContext = (void *)23123;
		HPMSdkSession *pSession= HPMSdkSession::SessionOpen(hpm_str(""), 0, hpm_str(""), hpm_str(""), hpm_str(""), this, NULL, true, EHPMSdkDebugMode_Off, _pClientData, 0, hpm_str(""), HPMSystemString(), NULL);
		SessionManager::getInstance().Initialize(pSession);
		// Register the event handlers that will be adding items to the right click menues and handling events coming from them. 
		m_EventHandlers.push_back(new CTCMTestCycleEventHandler(pSession));
		m_EventHandlers.push_back(new CTCMRequirementsEventHandler(pSession));
	}
	catch (HPMSdkException &_Exception)
	{
		HPMString ErrorStr = _Exception.GetAsString();

		wstringstream Stream;
		Stream << hpm_str("Error initializing TCM Extension Client Plugin:\r\n\r\n");
		Stream << ErrorStr;
		MessageBox(NULL, Stream.str().c_str(), hpm_str("TCM Extension Client Plugin Error"), MB_OK|MB_ICONERROR);
	}
	catch (HPMSdkCppException &_Exception)
	{
		wstringstream Stream; 
		Stream << hpm_str("Error initializing TCM Extension Client Plugin:\r\n\r\n");
		Stream << _Exception.what();
		MessageBox(NULL, Stream.str().c_str(), hpm_str("TCM Extension Client Plugin Error"), MB_OK|MB_ICONERROR);
	}
}

CHansoftTCMExtension_ClientPlugin::~CHansoftTCMExtension_ClientPlugin()
{ 
	for (unsigned i = 0; i < m_EventHandlers.size(); i++)
	{
		delete m_EventHandlers[i];
	}
	m_pDynamicHelper.reset();
	if (SessionManager::getInstance().Session())
	{
		HPMSdkSession::SessionDestroy(SessionManager::getInstance().Session());
		SessionManager::getInstance().Initialize(nullptr);
	}
}

void CHansoftTCMExtension_ClientPlugin::On_ProcessError(EHPMError _Error)
{
}

void CHansoftTCMExtension_ClientPlugin::On_Callback(const HPMChangeCallbackData_ClientSyncDone &_Data)
{
	try
	{

		m_pDynamicHelper = shared_ptr<CDynamicHelper>(new CDynamicHelper());
		m_pDynamicHelper->m_RightClickSubscription = SessionManager::getInstance().Session()->GlobalRegisterForRightClickNotifications(NULL);
		m_pDynamicHelper->m_DynamicUpdateSubscription = SessionManager::getInstance().Session()->GlobalRegisterForDynamicCustomSettingsNotifications(s_pluginIdentifier + hpm_str("."), m_UserContext);
	}
	catch (const HPMSdk::HPMSdkException &_Exception)
	{
		if (_Exception.GetError() == EHPMError_ConnectionLost)
			return;
	}
	catch (const HPMSdk::HPMSdkCppException &)
	{
	}
}


/*********************************************
* Called when the right click menu is displayed.
* Params:
*	_Data - the data regarding the right click event fired (@see HPMChangeCallbackData_RightClickDisplayTaskMenu)
*********************************************/
void CHansoftTCMExtension_ClientPlugin::On_Callback(const HPMChangeCallbackData_RightClickDisplayTaskMenu &_Data)
{
	try
	{
		if (_Data.m_SelectedTasks.size() > 0)
		{
			m_LastSelectedProjectUID = SessionManager::getInstance().Session()->UtilGetRealProjectIDFromProjectID(_Data.m_ProjectID);
			m_LastSelectedTasks = _Data.m_SelectedTasks;
			HPMString projectName = SessionManager::getInstance().Session()->ProjectGetProperties(m_LastSelectedProjectUID).m_Name;
			CTCMEventHandler::HansoftSection section;
			if (SessionManager::getInstance().Session()->UtilIsIDBacklogProject(_Data.m_ProjectID))
			{
				section = CTCMEventHandler::HansoftSection::BACKLOG;
			}
			else if (SessionManager::getInstance().Session()->UtilIsIDProject(_Data.m_ProjectID))
			{
				section = CTCMEventHandler::HansoftSection::SCHEDULE;
			}
			else
			{
				section = CTCMEventHandler::HansoftSection::QA;
			}

			for (unsigned i = 0; i < m_EventHandlers.size(); i++)
			{
				m_EventHandlers[i]->addYourStuffToRightClick(_Data.m_RightClickContext, projectName, s_pluginIdentifier, section, m_LastSelectedTasks);
			}
		}
	}
	catch (const HPMSdk::HPMSdkException &_Exception)
	{
		if (_Exception.GetError() == EHPMError_ConnectionLost)
			return;
	}
	catch (const HPMSdk::HPMSdkCppException &)
	{
	}
}



/*********************************************
* Called when a right click menu item is selected
* Params:
*	_Data - the data regarding the selected item (@see HPMChangeCallbackData_RightClickMenuItemSelected)
*********************************************/
void CHansoftTCMExtension_ClientPlugin::On_Callback(const HPMChangeCallbackData_RightClickMenuItemSelected &_Data)
{
	try
	{			
		if (_Data.m_UniqueName.find(s_pluginIdentifier) == 0)
		{
			HPMString action = _Data.m_UniqueName.substr(s_pluginIdentifier.length());
			for (unsigned i = 0; i < m_EventHandlers.size(); i++)
			{
				if (m_EventHandlers[i]->handleThis(action, m_LastSelectedTasks, m_LastSelectedProjectUID))
					break;
			}
		}
	}
	catch (const HPMSdk::HPMSdkException &_Exception)
	{
		if (_Exception.GetError() == EHPMError_ConnectionLost)
			return;
	}
	catch (const HPMSdk::HPMSdkCppException &)
	{
	}
}

void CHansoftTCMExtension_ClientPlugin::On_Callback(const HPMChangeCallbackData_DynamicCustomSettingsValueChanged &_Data)
{
	try
	{
		if (_Data.m_UserContext != m_UserContext)
			return;
		HPMString dataID = _Data.m_Path;
		for (unsigned i = 0; i < m_EventHandlers.size(); i++)
		{
			if (m_EventHandlers[i]->customDataUpdated(dataID, _Data.m_Value))
				break;
		}
	}
	catch (const HPMSdk::HPMSdkException &_Exception)
	{
		if (_Exception.GetError() == EHPMError_ConnectionLost)
			return;
	}
	catch (const HPMSdk::HPMSdkCppException &)
	{
	}
}

void CHansoftTCMExtension_ClientPlugin::On_Callback(const HPMChangeCallbackData_DynamicCustomSettingsNotification &_Data)
{
	try
	{
		if (_Data.m_UserContext != m_UserContext || !_Data.m_TabIdentifiers.size() == 1)
			return;
		if (_Data.m_Notification == EHPMDynamicCustomSettingsNotification_DialogEndedOk)
		{
			HPMString dialog = _Data.m_TabIdentifiers[0];
			for (unsigned i = 0; i < m_EventHandlers.size(); i++)
			{
				if (m_EventHandlers[i]->customDialogClosed(_Data.m_TabIdentifiers[0], m_LastSelectedTasks, m_LastSelectedProjectUID))
					break;
			}
		}
	}
	catch (const HPMSdk::HPMSdkException &_Exception)
	{
		if (_Exception.GetError() == EHPMError_ConnectionLost)
			return;
	}
	catch (const HPMSdk::HPMSdkCppException &)
	{
	}
}