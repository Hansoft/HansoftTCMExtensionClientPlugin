#include "TCMRequirementsEventHandler.h"

bool CTCMRequirementsEventHandler::handleThis(HPMString action, std::vector<HPMUniqueID> &selectedTasks, HPMUniqueID projectUID)
{
	if (action == s_addSuiteItem)
	{
		return true;
	}
	else if (action == s_addUserStoryItem)
	{
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
