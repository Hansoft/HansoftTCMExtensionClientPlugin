#include "TCMTestCycleEventHandler.h"
#include "HansoftUtils.h"


CTCMTestCycleEventHandler::CTCMTestCycleEventHandler(HPMSdkSession *_pSession) : CTCMEventHandler(_pSession)
{
	m_bugTitle = hpm_str("");
	m_addBugDialogue =
		hpm_str("	DialogName \"Report bug\"\r\n")
		hpm_str("	Item Tab\r\n")
		hpm_str("	{\r\n")
		hpm_str("		Identifier \"") + s_reportBugDialog+hpm_str("\"\r\n")
		hpm_str("		Name \"Info\"\r\n")
		hpm_str("		InfoText \"Bug reporting dialog\"\r\n")
		hpm_str("		LayoutStyle \"VerticalList\"\r\n")
		hpm_str("		Item FormLayout\r\n")
		hpm_str("		{\r\n")
		hpm_str("			Identifier \"Form\"\r\n")
		hpm_str("			Item Edit\r\n")
		hpm_str("			{\r\n")
		hpm_str("				Identifier \"BugTitle\"\r\n")
		hpm_str("				Name \"Bug title:\"\r\n")
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
		hpm_str("			Identifier \"BugInfo\"\r\n")
		hpm_str("			DefaultValue \"The new bug will be added to the bug section and linked to the test case in both linked to item and in the comment stream.\"\r\n")
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
bool CTCMTestCycleEventHandler::customDataUpdated(HPMString dataID, HPMString value)  
{
	if (dataID == s_reportBugDialog + s_reportBugDialogBugTitle)
	{
		m_bugTitle = value;
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
bool CTCMTestCycleEventHandler::customDialogClosed(HPMString dialog, std::vector<HPMUniqueID> &selectedTasks, HPMUniqueID projectUID)
{
	if (dialog == s_reportBugDialog)
	{
		onReportBug(m_bugTitle, selectedTasks, projectUID);
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
bool CTCMTestCycleEventHandler::handleThis(HPMString action, std::vector<HPMUniqueID> &selectedTasks, HPMUniqueID projectUID)
{
	if (action == s_startCycleItem)
	{
		onStartTestCycle(selectedTasks, projectUID);
		return true;
	}
	else if (action == s_endCycleItem)
	{
		onEndTestCycle(selectedTasks, projectUID);
		return true;
	}
	else if (action == s_reportBugItem)
	{
		m_bugTitle = hpm_str("");

		HPMString initialValues =
			s_reportBugDialog+hpm_str("\r\n")
			hpm_str("{\r\n")
			hpm_str("}");
		m_pSession->GlobalDisplayCustomSettingsDialog
			(
			HPMUniqueID()							// Can be set to specific project
			, HPMUniqueID()							// Can be set to specific resource
			, m_addBugDialogue
			, initialValues
			)
			;
//		onReportBug(selectedTasks, projectUID);
		return true;
	}
	return false;
}


/*********************************************
* Adds items to the right click menu.
* Params:
*	rightClickContext - the context to add actions to
*	projectName - the name of the project where the right click menu was invoked
*   root -the root of this plugin
*   section - the section of the project we're currently in.
*   selectedTasks - the current selection of tasks
* Returns:
*	Returns true if the event handler wants to add something to the right click menu.
*********************************************/
bool CTCMTestCycleEventHandler::addYourStuffToRightClick(HPMRightClickContext rightClickContext, HPMString projectName, HPMString root, HansoftSection section, std::vector<HPMUniqueID> &selectedTasks)
{
	if (projectName == s_testCaseProjectName)
	{
		if (section == HansoftSection::BACKLOG)
		{
			// Add the possibility to start a test cycle to the right click menu
			m_pSession->GlobalAddRightClickMenuItem(rightClickContext, hpm_str(""), root + s_startCycleItem,
				m_pSession->LocalizationCreateUntranslatedStringFromString(s_startCycleMenuAlternative), NULL);
			return true;
		}
		else if (section == HansoftSection::SCHEDULE && selectedTasks.size() == 1)
		{
			HPMUniqueID taskID = m_pSession->TaskRefGetTask(selectedTasks[0]);
			//Only show end cycle alternative when a sprint is selected.
			if (m_pSession->TaskGetLockedType(taskID) == EHPMTaskLockedType_SprintItem)
			{
				// Add the possibility to end a test cycle to the right click menu
				m_pSession->GlobalAddRightClickMenuItem(rightClickContext, hpm_str(""), root + s_endCycleItem,
					m_pSession->LocalizationCreateUntranslatedStringFromString(s_endCycleMenuAlternative), NULL);
				return true;
			}
			if (m_pSession->TaskGetLockedType(taskID) == EHPMTaskLockedType_BacklogItem && !m_pSession->TaskRefUtilHasChildren((selectedTasks[0])))
			{
				// Add the possibility to report a bug if it's a backlog item that has no children.
				m_pSession->GlobalAddRightClickMenuItem(rightClickContext, hpm_str(""), root + s_reportBugItem,
					m_pSession->LocalizationCreateUntranslatedStringFromString(s_reportBugMenuAlternative), NULL);
				return true;
			}
		}
	}
	return false;
}



/*********************************************
* Called when the user has selected a number of items
* in the backlog of the test case management project and
* chosen to start a test cycle.
*********************************************/
void CTCMTestCycleEventHandler::onStartTestCycle(std::vector<HPMUniqueID> selectedTasks, HPMUniqueID projectUID)
{
	HPMUniqueID sprintRefID = HansoftUtils::createSprint(hpm_str("Test cycle"), 40, projectUID);
	HansoftUtils::commitBacklogItems(sprintRefID, projectUID, selectedTasks);
}



/*********************************************
* Called when the user invokes the end cycle menu alternative.
*********************************************/
void CTCMTestCycleEventHandler::onEndTestCycle(std::vector<HPMUniqueID> selectedTasks, HPMUniqueID projectUID)
{
	HPMTaskEnum childrenEnum = m_pSession->TaskRefUtilEnumChildren(selectedTasks[0], false);
	for (unsigned int i = 0; i < childrenEnum.m_Tasks.size(); ++i)
	{
		HansoftUtils::resetAndDeleteTask(childrenEnum.m_Tasks[i]);
	}

	// Delete the sprint
	HPMUniqueID taskID = m_pSession->TaskRefGetTask(selectedTasks[0]);
	m_pSession->TaskDelete(taskID);
}

/*********************************************
* Called when the user invokes the report bug menu alternative.
*********************************************/
void CTCMTestCycleEventHandler::onReportBug(HPMString bugName, std::vector<HPMUniqueID> selectedTasks, HPMUniqueID projectUID)
{
	HPMUniqueID bugRefID = HansoftUtils::createBug(bugName, projectUID);
	if (bugRefID.IsValid())
	{
		HPMUniqueID bugUID = m_pSession->TaskRefGetTask(bugRefID);
		std::wstringstream bugLink;
		bugLink << hpm_str("Task/") << bugUID.m_ID;
		HPMString URL = m_pSession->UtilGetHansoftURL(bugLink.str());
		URL = hpm_str("<URL=") + URL + hpm_str(">") + bugName;
		for (unsigned int i = 0; i < selectedTasks.size(); ++i)
		{
			HansoftUtils::postComment(selectedTasks[i], URL);
			HansoftUtils::createLink(selectedTasks[i], bugRefID);
			HansoftUtils::createLink(bugRefID, selectedTasks[i]);
		}
	}
}