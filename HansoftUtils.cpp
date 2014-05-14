#include "HansoftUtils.h"
#include "SessionManager.h"


/*********************************************
* Creates a new test cycle to commit selected test cases to
* Params:
*	name - the name of the new test cycle
*   duration - the number of days the cycle should last
*   projectUID - the id of the project to add the cycle in
* Returns:
*	The reference ID of the created sprint.
*********************************************/
HPMUniqueID HansoftUtils::createSprint(HPMString name, int duration, HPMUniqueID projectUID)
{
	HPMSdkSession *session = SessionManager::getInstance().Session();
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

	HPMChangeCallbackData_TaskCreateUnified TaskCreateReturn = session->TaskCreateUnifiedBlock(projectUID, NewTestCycle);

	// We know that we are only creating one item, so we'll get the identifier for that one.
	HPMUniqueID sprintRefID = TaskCreateReturn.m_Tasks[0].m_TaskRefID;
	HPMUniqueID sprintTaskID = session->TaskRefGetTask(sprintRefID);

	session->TaskSetFullyCreated(sprintTaskID);
	session->TaskSetDescription(sprintTaskID, name);
	session->TaskSetDuration(sprintTaskID, duration);
	return sprintRefID;
}

/*********************************************
* Creates an entry that can be commited to a sprint
* Params:
*	taskRefID - the ref id of the test case to create an entry for (should be a backlog item)
*   index - the local index of the task in the current list of test cases to add
*   previousItem - the previous item add the current one after
*   parent - a reference to the parent
* Returns:
*	A entry that can be used to commit an item to a sprint
*********************************************/
HPMTaskCreateUnifiedEntry HansoftUtils::createCommitEntry(HPMUniqueID taskRefID, unsigned index, HPMTaskCreateUnifiedReference &previousItem, HPMTaskCreateUnifiedReference &parent)
{
	HPMSdkSession *session = SessionManager::getInstance().Session();
	HPMTaskCreateUnifiedEntry entry;
	entry.m_bIsProxy = true;
	entry.m_PreviousRefID = previousItem;
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
*   index - the local index of the task in the current list of backlog items to addto add
*   previousItem - the previous item add the current one after
*   parent - a reference to the parent
* Returns:
*	A list of entries that can be used to commit a number of items to a sprint
*********************************************/
std::vector<HPMTaskCreateUnifiedEntry> HansoftUtils::createCommitEntryList(HPMUniqueID taskRefID, unsigned &index, HPMTaskCreateUnifiedReference &previousItem, HPMTaskCreateUnifiedReference parent)
{
	HPMSdkSession *session = SessionManager::getInstance().Session();
	std::vector<HPMTaskCreateUnifiedEntry> testCases;
	HPMTaskCreateUnifiedEntry entry = createCommitEntry(taskRefID, index, previousItem, parent);
	// We need to set the parent to be using local ID as it isn't created yet.
	parent.m_bLocalID = true;
	parent.m_RefID = index;
	index++;
	testCases.push_back(entry);
	if (session->TaskRefUtilHasChildren(taskRefID))
	{
		previousItem = parent;
		HPMTaskEnum childrenEnum = session->TaskRefUtilEnumChildren(taskRefID, false);
		for (unsigned int i = 0; i < childrenEnum.m_Tasks.size(); ++i)
		{
			std::vector<HPMTaskCreateUnifiedEntry> childrenEntries = createCommitEntryList(childrenEnum.m_Tasks[i], index, previousItem, parent);
			index++;
			testCases.insert(testCases.end(), childrenEntries.begin(), childrenEntries.end());

			// Set the id to the current task so that the next created task will end up after this one. 
			previousItem.m_bLocalID = true;
			previousItem.m_RefID = index;
		}
	}
	return testCases;
}

/*********************************************
* Commits the list of backlog items to the sprint. Will handle if both parent and children exists in the list
* by committing the parent with all its children.
* Params:
*	sprintRefID - the sprint to commit the tasks to
*	projectUID - the project the sprint exists in
*   backlogItems - the list of backlog item ID's to commit
*********************************************/
void HansoftUtils::commitBacklogItems(HPMUniqueID sprintRefID, HPMUniqueID projectUID, vector<HPMUniqueID> backlogItems)
{
	HPMSdkSession *session = SessionManager::getInstance().Session();
	HPMTaskCreateUnified committedBacklogItems;
	HPMTaskCreateUnifiedReference sprintRefObj;
	sprintRefObj.m_bLocalID = false;
	sprintRefObj.m_RefID = sprintRefID;


	// Loop over all the created tasks and add them to the newly created test cycle
	HPMTaskCreateUnifiedReference prevRef = sprintRefObj;
	unsigned index = 0;
	for (unsigned int i = 0; i < backlogItems.size(); ++i)
	{
		if (!HansoftUtils::isParentIsList(backlogItems[i], backlogItems))
		{
			std::vector<HPMTaskCreateUnifiedEntry> testCaseEntries = createCommitEntryList(backlogItems[i], index, prevRef, sprintRefObj);
			committedBacklogItems.m_Tasks.insert(committedBacklogItems.m_Tasks.end(), testCaseEntries.begin(), testCaseEntries.end());

			// Set the id to the current task so that the next created task will end up after this one. 
			prevRef.m_bLocalID = true;
			prevRef.m_RefID = index;
		}
	}
	session->TaskCreateUnifiedBlock(projectUID, committedBacklogItems);
}


/*********************************************
* Checks if a parent (at any level above)  is in the list of selected tasks
* Params:
*	taskRefID - the ref id to check for parents.
*   tasks - the of task to see if the parent exists in
* Returns:
*	true if a parent exists in the list
*********************************************/
bool HansoftUtils::isParentIsList(HPMUniqueID taskRefID, std::vector<HPMUniqueID> tasks)
{
	HPMSdkSession *session = SessionManager::getInstance().Session();
	HPMUniqueID parentRefID = session->TaskRefUtilGetParent(taskRefID);
	//Check that we aren't at the top of the hierarchy and have recieved the project as the parent
	HPMUniqueID container = session->TaskRefGetContainer(taskRefID);
	if (parentRefID == container)
		return false;
	for (unsigned int i = 0; i < tasks.size(); ++i)
	{
		if (tasks[i] == parentRefID)
			return true;
	}

	return isParentIsList(parentRefID, tasks);
}

/*********************************************
* Resets the workflow status and returns the item to the backlog.
* Params:
*	taskRefID - the ref id delete and reset.
*********************************************/
void HansoftUtils::resetAndDeleteTask(HPMUniqueID taskRefID)
{
	HPMSdkSession *session = SessionManager::getInstance().Session();
	HPMTaskEnum childrenEnum = session->TaskRefUtilEnumChildren(taskRefID, false);
	for (unsigned int i = 0; i < childrenEnum.m_Tasks.size(); ++i)
	{
		resetAndDeleteTask(childrenEnum.m_Tasks[i]);
	}

	HPMUniqueID taskID = session->TaskRefGetTask(taskRefID);
	int workFlowID = session->TaskGetWorkflow(taskID);
	if (workFlowID != -1)
	{
		//TODO: Actually reset the workflow to a state name.
		session->TaskSetWorkflowStatus(taskID, 1, EHPMTaskSetStatusFlag_None);
	}
	HPMUniqueID proxyID = session->TaskGetProxy(taskID);
	if (proxyID.IsValid())
		session->TaskDelete(proxyID);
}

/*********************************************
* Searches through the comments of a task and finds the comment with the highest ID.
* Params:
*	taskID - the task to find the comment for
* Returns:
*   the index of the latest comment
*********************************************/
HPMInt32 HansoftUtils::findGreatestCommentPostID(HPMUniqueID taskID)
{
	HPMSdkSession *session = SessionManager::getInstance().Session();
	HPMTaskCommentEnum comments = session->TaskEnumComments(taskID);
	HPMInt32 greatestComment = -1;
	for (unsigned int i = 0; i < comments.m_Comments.size(); ++i)
	{
		if (comments.m_Comments[i] > greatestComment)
		{
			greatestComment = comments.m_Comments[i];
		}
	}
	return greatestComment;
}


/*********************************************
* Creates a link between the two incoming items.
* Params:
*	fromRef - starting point
*	toRef - end point
*********************************************/
void HansoftUtils::createLink(HPMUniqueID fromRef, HPMUniqueID toRef)
{
	HPMSdkSession *session = SessionManager::getInstance().Session();
	// Add a link from the Epic to the created feature
	HPMUniqueID fromID = session->TaskRefGetTask(fromRef);
	/*if (m_pSession->UtilIsIDTaskRef(selectedRequirementUID))
	{
		LinkedTo.m_LinkedTo[0].m_LinkedTo = selectedRequirementUID;
	}
	else
	{
		LinkedTo.m_LinkedTo[0].m_LinkedTo = m_pSession->TaskGetMainReference(selectedRequirementUID);
	}*/
	HPMTaskLinkedTo linkedItems = session->TaskGetLinkedTo(fromID);
	HPMUInt32 newSize = linkedItems.m_LinkedTo.size() + 1;
	linkedItems.m_LinkedTo.resize(newSize);
	linkedItems.m_LinkedTo[newSize - 1].m_LinkedTo = toRef;
	linkedItems.m_LinkedTo[newSize - 1].m_LinkedToType = EHPMTaskLinkedToLinkType_TaskItemOrBug;
	session->TaskSetLinkedTo(fromID, linkedItems);
}



/*********************************************
* Creates a new bug.
* Params:
*	name - the name of the new bug
*	projectUID - project to create the bug in
* Returns:
*   the ref id of the new bug
*********************************************/
HPMUniqueID HansoftUtils::createBug(HPMString name, HPMUniqueID projectUID)
{
	HPMSdkSession *session = SessionManager::getInstance().Session();
	HPMUniqueID projectQAUID = session->ProjectUtilGetQA(projectUID);
	// Create a new one
	HPMTaskCreateUnified createData;
	createData.m_Tasks.resize(1);

	// Set previous to -1 to make it the top task.
	HPMTaskCreateUnifiedReference prevRefID;
	prevRefID.m_RefID = -1;
	HPMTaskCreateUnifiedReference prevWorkPrioRefID;
	prevWorkPrioRefID.m_RefID = -2;

	createData.m_Tasks[0].m_LocalID = 1;
	createData.m_Tasks[0].m_PreviousRefID = prevRefID;
	createData.m_Tasks[0].m_PreviousWorkPrioRefID = prevWorkPrioRefID;

	HPMChangeCallbackData_TaskCreateUnified TaskCreateReturn = session->TaskCreateUnifiedBlock(projectQAUID, createData);
	if (TaskCreateReturn.m_Tasks.size() == 1)
	{
		// The returned is a bug ref in the project container. We need the task id not the reference id.
		HPMUniqueID bugRefID = TaskCreateReturn.m_Tasks[0].m_TaskRefID;
		HPMUniqueID bugUID = session->TaskRefGetTask(bugRefID);
		session->TaskSetDescription(bugUID, name);
		// When we set fully created the task becomes visible to users.
		session->TaskSetFullyCreated(bugUID);
		return bugRefID;
	}
	return -1;
}


/*********************************************
* Posts a comment at the top of the list of comments for the incoming task.
* Params:
*	taskRefID - the task to comment at
*	commentStr - the comment to post
*********************************************/
void HansoftUtils::postComment(HPMUniqueID taskRefID, HPMString commentStr)
{
	HPMSdkSession *session = SessionManager::getInstance().Session();
	HPMUniqueID taskUID = session->TaskRefGetTask(taskRefID);
	HPMInt32 greatestComment = findGreatestCommentPostID(taskUID);
	HPMTaskComment comment;
	comment.m_ParentID = greatestComment;
	comment.m_Flags = EHPMTaskCommentFlag_IsPosted;
	comment.m_MessageText = commentStr;
	comment.m_PostedByResourceID = session->ResourceGetLoggedIn();
	HPMInt32 NewCommentID = session->TaskCreateCommentBlock(taskUID, comment);
	session->TaskNotifyCommentPosted(taskUID, NewCommentID, false);
}


/*********************************************
* The projects in the database that the SessionManager is connected to.
* Params:
*	projectName - name of the project to look for
* Returns:
*  the ID of the project with the corresponding name. -1 if it doesn't exist.
**********************************************/
HPMUniqueID HansoftUtils::findProject(HPMString projectName)
{
	HPMSdkSession *session = SessionManager::getInstance().Session();
	HPMProjectEnum projectIDs = session->ProjectEnum();
	for (int i = 0; i < projectIDs.m_Projects.size(); ++i)
	{
		HPMProjectProperties properties = session->ProjectGetProperties(projectIDs.m_Projects[i]);
		if (properties.m_Name == projectName)
			return projectIDs.m_Projects[i];
	}
	return -1;
}


/*********************************************
* Creates a backlog item in the project with the specified name, under the parent object (if not empty string).
* The items will be called the item name.
* Params:
*	projectName - name of the project to create the item in the backlog
*	parentName - name of the parent item to put the item under
*	itemName - name for the newly created item
* Returns:
*  the ref ID of the newly created item. -1 if not successful.
**********************************************/
HPMUniqueID HansoftUtils::createBacklogItem(HPMString projectName, HPMString parentName, HPMString itemName)
{
	HPMSdkSession *session = SessionManager::getInstance().Session();
	HPMUniqueID projectID = HansoftUtils::findProject(projectName);
	if (!projectID.IsValid())
		return -1;
	HPMProjectProperties projectProp = session->ProjectGetProperties(projectID);
	HPMUniqueID productBacklogUID = session->ProjectUtilGetBacklog(projectID);
	HPMTaskEnum Tasks = session->TaskEnum(productBacklogUID);
	for (HPMUInt32 j = 0; j < Tasks.m_Tasks.size(); ++j)
	{
		HPMUniqueID parentBacklogID = Tasks.m_Tasks[j];
		// TODO: Handle if no parent is specified
		if (session->TaskGetDescription(parentBacklogID).compare(parentName) == 0)
		{
			HPMTaskCreateUnified newBacklogItem;
			newBacklogItem.m_Tasks.resize(1);
			newBacklogItem.m_Tasks[0].m_bIsProxy = false;
			newBacklogItem.m_Tasks[0].m_LocalID = -1;
			newBacklogItem.m_Tasks[0].m_TaskLockedType = EHPMTaskLockedType_BacklogItem;

			HPMTaskCreateUnifiedReference prevRefID;
			prevRefID.m_bLocalID = false;
			prevRefID.m_RefID = -1;
			newBacklogItem.m_Tasks[0].m_PreviousRefID = prevRefID;

			HPMTaskCreateUnifiedReference prevWorkPrioRefID;
			prevWorkPrioRefID.m_bLocalID = false;
			prevWorkPrioRefID.m_RefID = -2;
			newBacklogItem.m_Tasks[0].m_PreviousWorkPrioRefID = prevWorkPrioRefID;

			HPMTaskCreateUnifiedReference parentBacklogTaskRefID;
			parentBacklogTaskRefID.m_bLocalID = false;
			parentBacklogTaskRefID.m_RefID = session->TaskGetMainReference(parentBacklogID);
			newBacklogItem.m_Tasks[0].m_ParentRefIDs.push_back(parentBacklogTaskRefID);

			newBacklogItem.m_Tasks[0].m_NonProxy_ReuseID = 0;
			newBacklogItem.m_Tasks[0].m_NonProxy_WorkflowID = 0xffffffff;

			HPMChangeCallbackData_TaskCreateUnified TaskCreateReturn = session->TaskCreateUnifiedBlock(productBacklogUID, newBacklogItem);
			if (newBacklogItem.m_Tasks.size() == 1)
			{
				// The returned is a task ref in the project container. We need the task id not the reference id.
				HPMUniqueID newTestSuiteTaskRefID = TaskCreateReturn.m_Tasks[0].m_TaskRefID;

				HPMUniqueID newTestSuiteTaskID = session->TaskRefGetTask(newTestSuiteTaskRefID);

				// Set the name and the status of the test suite
				session->TaskSetDescription(newTestSuiteTaskID, itemName);
				session->TaskSetStatus(newTestSuiteTaskID, EHPMTaskStatus_NotDone, true, EHPMTaskSetStatusFlag_All);

				session->TaskSetFullyCreated(newTestSuiteTaskID);
				return newTestSuiteTaskRefID;
			}
			else
				return -1;
		}
	}
	return -1;
}