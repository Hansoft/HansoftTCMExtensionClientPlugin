#include "TCMTestCycleEventHandler.h"
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
		onReportBug(selectedTasks, projectUID);
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
* Creates a new test cycle to commit selected test cases to
* Params:
*	name - the name of the new test cycle
*   duration - the number of days the cycle should last
*   projectUID - the id of the project to add the cycle in
* Returns:
*	The reference ID of the created sprint.
*********************************************/
HPMUniqueID CTCMTestCycleEventHandler::createTestCycle(HPMString name, int duration, HPMUniqueID projectUID)
{
	//Create a sprint to commit items to
	HPMTaskCreateUnified NewTestCycle;
	NewTestCycle.m_Tasks.resize(1);
	NewTestCycle.m_Tasks[0].m_bIsProxy = false;
	NewTestCycle.m_Tasks[0].m_LocalID = -1;
	NewTestCycle.m_Tasks[0].m_TaskLockedType = EHPMTaskLockedType_SprintItem;

	//The new test cycle will end up at the top of the schedule.
	HPMTaskCreateUnifiedReference PrevRefID;
	PrevRefID.m_bLocalID = false;
	PrevRefID.m_RefID = -1;
	NewTestCycle.m_Tasks[0].m_PreviousRefID = PrevRefID;

	HPMTaskCreateUnifiedReference PrevWorkPrioRefID;
	PrevWorkPrioRefID.m_bLocalID = false;
	PrevWorkPrioRefID.m_RefID = -2;
	NewTestCycle.m_Tasks[0].m_PreviousWorkPrioRefID = PrevWorkPrioRefID;

	HPMChangeCallbackData_TaskCreateUnified TaskCreateReturn = m_pSession->TaskCreateUnifiedBlock(projectUID, NewTestCycle);
	
	// We know that we are only creating one item, so we'll get the identifier for that one.
	HPMUniqueID sprintRefID = TaskCreateReturn.m_Tasks[0].m_TaskRefID;
	HPMUniqueID sprintTaskID = m_pSession->TaskRefGetTask(sprintRefID);

	m_pSession->TaskSetFullyCreated(sprintTaskID);
	m_pSession->TaskSetDescription(sprintTaskID, name);
	m_pSession->TaskSetDuration(sprintTaskID, duration);
	return sprintRefID;
}

/*********************************************
* Creates an entry that can be commited to a test cycle
* Params:
*	taskRefID - the ref id of the test case to create an entry for
*   index - the local index of the task in the current list of test cases to add
*   previousTestCase - the previous tesk case to add the current one after
*   parent - a reference to the parent
* Returns:
*	A entry that can be used to commit a test case to a test cycle
*********************************************/
HPMTaskCreateUnifiedEntry CTCMTestCycleEventHandler::createEntryForTestCase(HPMUniqueID taskRefID, unsigned index, HPMTaskCreateUnifiedReference &previousTestCase, HPMTaskCreateUnifiedReference &parent)
{
	HPMTaskCreateUnifiedEntry entry;
	entry.m_bIsProxy = true;
	entry.m_PreviousRefID = previousTestCase;
	entry.m_LocalID = index;
	entry.m_Proxy_ReferToRefTaskID = taskRefID;

	entry.m_TaskLockedType = EHPMTaskLockedType_BacklogItem;
	entry.m_ParentRefIDs.push_back(parent);

	return entry;
}


/*********************************************
* Adds the specified test case and any potential children to a list of cases to commit
* Params:
*	taskRefID - the ref id of the test case to create an entry for
*   index - the local index of the task in the current list of test cases to add
*   previousTestCase - the previous tesk case to add the current one after
*   parent - a reference to the parent
* Returns:
*	A list of entries that can be used to commit a test case to a test cycle
*********************************************/
std::vector<HPMTaskCreateUnifiedEntry> CTCMTestCycleEventHandler::createTestCaseEntryList(HPMUniqueID taskRefID, unsigned &index, HPMTaskCreateUnifiedReference &previousTestCase, HPMTaskCreateUnifiedReference parent)
{
	std::vector<HPMTaskCreateUnifiedEntry> testCases;
	HPMTaskCreateUnifiedEntry entry = createEntryForTestCase(taskRefID, index, previousTestCase, parent);
	// We need to set the parent to be using local ID as it isn't created yet.
	parent.m_bLocalID = true;
	parent.m_RefID = index;
	index++;
	testCases.push_back(entry);
	if (m_pSession->TaskRefUtilHasChildren(taskRefID))
	{
		previousTestCase = parent;
		HPMTaskEnum childrenEnum = m_pSession->TaskRefUtilEnumChildren(taskRefID, false);
		for (unsigned int i = 0; i < childrenEnum.m_Tasks.size(); ++i)
		{
			std::vector<HPMTaskCreateUnifiedEntry> childrenEntries = createTestCaseEntryList(childrenEnum.m_Tasks[i], index, previousTestCase, parent);
			index++;
			testCases.insert(testCases.end(), childrenEntries.begin(), childrenEntries.end());

			// Set the id to the current task so that the next created task will end up after this one. 
			previousTestCase.m_bLocalID = true;
			previousTestCase.m_RefID = index;
		}
	}
	return testCases;
}

/*********************************************
* Checks if a parent (at any level above)  is in the list of selected tasks
* Params:
*	taskRefID - the ref id to check for parents.
*   selectedTasks - the lsit of selected tasks
* Returns:
*	true if a parent exists in the list
*********************************************/
bool CTCMTestCycleEventHandler::isParentIsList(HPMUniqueID taskRefID, std::vector<HPMUniqueID> selectedTasks)
{
	HPMUniqueID parentRefID = m_pSession->TaskRefUtilGetParent(taskRefID);
	//Check that we aren't at the top of the hierarchy and have recieved the project as the parent
	HPMUniqueID container = m_pSession->TaskRefGetContainer(taskRefID);
	if (parentRefID == container)
		return false;
	for (unsigned int i = 0; i < selectedTasks.size(); ++i)
	{
		if (selectedTasks[i] == parentRefID)
			return true;
	}

	return isParentIsList(parentRefID, selectedTasks);
}

/*********************************************
* Called when the user has selected a number of items
* in the backlog of the test case management project and
* chosen to start a test cycle.
*********************************************/
void CTCMTestCycleEventHandler::onStartTestCycle(std::vector<HPMUniqueID> selectedTasks, HPMUniqueID projectUID)
{
	HPMUniqueID sprintRefID = createTestCycle(hpm_str("Test cycle"), 40, projectUID);

	HPMTaskCreateUnified committedTestCases;
	HPMTaskCreateUnifiedReference sprintRefObj;
	sprintRefObj.m_bLocalID = false;
	sprintRefObj.m_RefID = sprintRefID;

	// Loop over all the created tasks and add them to the newly created test cycle
	HPMTaskCreateUnifiedReference prevRef = sprintRefObj;
	unsigned index = 0;
	for (unsigned int i = 0; i < selectedTasks.size(); ++i)
	{
		if (!isParentIsList(selectedTasks[i], selectedTasks))
		{
			std::vector<HPMTaskCreateUnifiedEntry> testCaseEntries = createTestCaseEntryList(selectedTasks[i], index, prevRef, sprintRefObj);
			committedTestCases.m_Tasks.insert(committedTestCases.m_Tasks.end(), testCaseEntries.begin(), testCaseEntries.end());

			// Set the id to the current task so that the next created task will end up after this one. 
			prevRef.m_bLocalID = true;
			prevRef.m_RefID = index;
		}
	}
	m_pSession->TaskCreateUnifiedBlock(projectUID, committedTestCases);
}



/*********************************************
* Called when the user invokes the end cycle menu alternative.
*********************************************/
void CTCMTestCycleEventHandler::onEndTestCycle(std::vector<HPMUniqueID> selectedTasks, HPMUniqueID projectUID)
{
	HPMTaskEnum childrenEnum = m_pSession->TaskRefUtilEnumChildren(selectedTasks[0], false);
	for (unsigned int i = 0; i < childrenEnum.m_Tasks.size(); ++i)
	{
		resetAndDeleteTask(childrenEnum.m_Tasks[i]);
	}

	HPMUniqueID taskID = m_pSession->TaskRefGetTask(selectedTasks[0]);
	m_pSession->TaskDelete(taskID);
}

/*********************************************
* Resets the workflow status and returns the item to the backlog.
* Params:
*	taskRefID - the ref id delete and reset.
*********************************************/

void CTCMTestCycleEventHandler::resetAndDeleteTask(HPMUniqueID taskRefID)
{
	HPMTaskEnum childrenEnum = m_pSession->TaskRefUtilEnumChildren(taskRefID, false);
	for (unsigned int i = 0; i < childrenEnum.m_Tasks.size(); ++i)
	{
		resetAndDeleteTask(childrenEnum.m_Tasks[i]);
	}

	HPMUniqueID taskID = m_pSession->TaskRefGetTask(taskRefID);
	int workFlowID = m_pSession->TaskGetWorkflow(taskID);
	if (workFlowID != -1)
	{
		//TODO: Actually reset the workflow to a state name.
		m_pSession->TaskSetWorkflowStatus(taskID, 1, EHPMTaskSetStatusFlag_None);
	}
	HPMUniqueID proxyID = m_pSession->TaskGetProxy(taskID);
	if (proxyID.IsValid())
		m_pSession->TaskDelete(proxyID);
}

/*********************************************
* Called when the user invokes the report bug menu alternative.
*********************************************/
void CTCMTestCycleEventHandler::onReportBug(std::vector<HPMUniqueID> selectedTasks, HPMUniqueID projectUID)
{

}