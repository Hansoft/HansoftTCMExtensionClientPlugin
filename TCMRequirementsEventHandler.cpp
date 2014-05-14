#include "TCMRequirementsEventHandler.h"
#include "HansoftUtils.h"


CTCMRequirementsEventHandler::CTCMRequirementsEventHandler(HPMSdkSession *_pSession) : CTCMEventHandler(_pSession)
{
	m_userStory = hpm_str("");
	m_addUserStoryDialogue =
		hpm_str("	DialogName \"Add user story\"\r\n")
		hpm_str("	Item Tab\r\n")
		hpm_str("	{\r\n")
		hpm_str("		Identifier \"") + s_addUserStoryDialog + hpm_str("\"\r\n")
		hpm_str("		Name \"Info\"\r\n")
		hpm_str("		InfoText \"User story dialog\"\r\n")
		hpm_str("		LayoutStyle \"VerticalList\"\r\n")
		hpm_str("		Item FormLayout\r\n")
		hpm_str("		{\r\n")
		hpm_str("			Identifier \"Form\"\r\n")
		hpm_str("			Item Edit\r\n")
		hpm_str("			{\r\n")
		hpm_str("				Identifier \"UserStory\"\r\n")
		hpm_str("				Name \"User story:\"\r\n")
		hpm_str("				DefaultValue \"As a .... I expect to be able to ..... so that ......\"\r\n")
		hpm_str("				Password 0\r\n")
		hpm_str("			}\r\n")
		hpm_str("		}\r\n")
		hpm_str("		Item FormLayout\r\n")
		hpm_str("		{\r\n")
		hpm_str("			Identifier \"Form2\"\r\n")
		hpm_str("		Item StaticText\r\n")
		hpm_str("		{\r\n")
		hpm_str("			Name \"Information\"\r\n")
		hpm_str("			Identifier \"UserStoryInfo\"\r\n")
		hpm_str("			DefaultValue \"The new user story will be added to the development project and linked to the requirement project. Data will propagate up into the requirement.\"\r\n")
		hpm_str("		}\r\n")
		hpm_str("		}\r\n")
		hpm_str("	}\r\n");

	m_testSuite = hpm_str("");
	m_addTestSuiteDialogue =
		hpm_str("	DialogName \"Add test suite\"\r\n")
		hpm_str("	Item Tab\r\n")
		hpm_str("	{\r\n")
		hpm_str("		Identifier \"") + s_addTestSuiteDialog + hpm_str("\"\r\n")
		hpm_str("		Name \"Info\"\r\n")
		hpm_str("		InfoText \"User test suite dialog\"\r\n")
		hpm_str("		LayoutStyle \"VerticalList\"\r\n")
		hpm_str("		Item FormLayout\r\n")
		hpm_str("		{\r\n")
		hpm_str("			Identifier \"Form\"\r\n")
		hpm_str("			Item Edit\r\n")
		hpm_str("			{\r\n")
		hpm_str("				Identifier \"TestSuite\"\r\n")
		hpm_str("				Name \"Test suite description:\"\r\n")
		hpm_str("				DefaultValue \"\"\r\n")
		hpm_str("				Password 0\r\n")
		hpm_str("			}\r\n")
		hpm_str("		}\r\n")
		hpm_str("		Item FormLayout\r\n")
		hpm_str("		{\r\n")
		hpm_str("			Identifier \"Form2\"\r\n")
		hpm_str("		Item StaticText\r\n")
		hpm_str("		{\r\n")
		hpm_str("			Name \"Information\"\r\n")
		hpm_str("			Identifier \"TestSuiteInfo\"\r\n")
		hpm_str("			DefaultValue \"The new test suite will be added to the test case project and linked to the requirement project. Data will propagate up into the requirement.\"\r\n")
		hpm_str("		}\r\n")
		hpm_str("		}\r\n")
		hpm_str("	}\r\n");
};


/*********************************************
* Called when a custom dialog data has changed
* Params:
*	dataID - the id of the data that has changed
*	value - the new value for the data
* Returns:
*	Returns true if the event handler is interested in this data update
*********************************************/
bool CTCMRequirementsEventHandler::customDataUpdated(HPMString dataID, HPMString value)
{
	if (dataID == s_addTestSuiteDialog + s_addTestSuiteDialogTestSuite)
	{
		m_testSuite = value;
		return true;
	}
	else if (dataID == s_addUserStoryDialog + s_addUserStoryDialogUserStory)
	{
		m_userStory = value;
		return true;
	}
	return false;
}

/*********************************************
* Called when a custom dialog has exited with ok
* Params:
*	dialog - the dialog that has been closed
*   selectedTasks - the current selection of tasks
*   projectUID - the unique identifier for the current project.
* Returns:
*	Returns true if the event handler is interested in this data update
*********************************************/
bool CTCMRequirementsEventHandler::customDialogClosed(HPMString dialog, std::vector<HPMUniqueID> &selectedTasks, HPMUniqueID projectUID)
{
	if (dialog == s_addTestSuiteDialog)
	{
		onCreateTestSuite(m_testSuite, selectedTasks, projectUID);
		return true;
	}
	else if (dialog == s_addUserStoryDialog)
	{
		onCreateUserStory(m_userStory, selectedTasks, projectUID);
		return true;
	}
	return false;
}

/*********************************************
* Handles the actions defined in s_startCycleItem and s_endCycleItem.
* Params:
*	action - the string defining the action that was invoked by the user.
*   selectedTasks - the current selection of tasks
*   projectUID - the unique identifier for the current project.
* Returns:
*	Returns true if the event handler will pick up this action
*********************************************/
bool CTCMRequirementsEventHandler::handleThis(HPMString action, std::vector<HPMUniqueID> &selectedTasks, HPMUniqueID projectUID)
{
	if (action == s_addSuiteItem)
	{
		m_testSuite = hpm_str("");

		HPMString initialValues =
			s_addTestSuiteDialog + hpm_str("\r\n")
			hpm_str("{\r\n")
			hpm_str("}");
		m_pSession->GlobalDisplayCustomSettingsDialog
			(
			HPMUniqueID()							// Can be set to specific project
			, HPMUniqueID()							// Can be set to specific resource
			, m_addTestSuiteDialogue
			, initialValues
			)
			;
		return true;
	}
	else if (action == s_addUserStoryItem)
	{
		m_userStory = hpm_str("");

		HPMString initialValues =
			s_addUserStoryDialog + hpm_str("\r\n")
			hpm_str("{\r\n")
			hpm_str("}");
		m_pSession->GlobalDisplayCustomSettingsDialog
			(
			HPMUniqueID()							// Can be set to specific project
			, HPMUniqueID()							// Can be set to specific resource
			, m_addUserStoryDialogue
			, initialValues
			)
			;
		return true;
	}
	return false;
}


bool CTCMRequirementsEventHandler::addYourStuffToRightClick(HPMRightClickContext rightClickContext, HPMString projectName, HPMString root, HansoftSection section, std::vector<HPMUniqueID> &selectedTasks)
{
	if (projectName == s_requirementsProjectName && section == HansoftSection::BACKLOG)
	{
		// Add the menu alternative to add a new test suite to the right click menu
		m_pSession->GlobalAddRightClickMenuItem(rightClickContext, hpm_str(""), root + s_addSuiteItem,
			m_pSession->LocalizationCreateUntranslatedStringFromString(s_addSuiteMenuAlternative), NULL);

		// Add the menu alternative to add a new user story to the right click menu
		m_pSession->GlobalAddRightClickMenuItem(rightClickContext, hpm_str(""), root + s_addUserStoryItem,
			m_pSession->LocalizationCreateUntranslatedStringFromString(s_addUserStoryAlternative), NULL);
		return true;
	}
	return false;
}

/*********************************************
* Called when the user invokes the add test suite alternative
*********************************************/
void CTCMRequirementsEventHandler::onCreateTestSuite(HPMString testSuiteName, std::vector<HPMUniqueID> selectedTasks, HPMUniqueID projectUID)
{
	HPMUniqueID testSuiteRefID = HansoftUtils::createBacklogItem(s_testCasesProjectName, s_testSuitesRootName, testSuiteName);
	if (testSuiteRefID.IsValid())
	{

		// Create the link from the created test suite to the requirement
		HansoftUtils::createLink(selectedTasks[0], testSuiteRefID);
		HansoftUtils::createLink(testSuiteRefID, selectedTasks[0]);

		MessageBox(NULL, hpm_str("The test suite was created"), hpm_str("Hansoft TCM plugin"), MB_OK | MB_ICONINFORMATION);
	}
}

/*********************************************
* Called when the user invokes the add user story alternative.
*********************************************/
void CTCMRequirementsEventHandler::onCreateUserStory(HPMString userStory, std::vector<HPMUniqueID> selectedTasks, HPMUniqueID projectUID)
{
	HPMUniqueID testSuiteRefID = HansoftUtils::createBacklogItem(s_developmentProjectName, s_developmentRootName, userStory);
	if (testSuiteRefID.IsValid())
	{
		// Create the link from the created test suite to the requirement
		HansoftUtils::createLink(selectedTasks[0], testSuiteRefID);
		HansoftUtils::createLink(testSuiteRefID, selectedTasks[0]);

		MessageBox(NULL, hpm_str("The user story was created"), hpm_str("Hansoft TCM plugin"), MB_OK | MB_ICONINFORMATION);
	}
}
