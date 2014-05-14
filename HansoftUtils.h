#pragma once
#include "../HansoftSDK/HPMSdkCpp.h"

using namespace std;
using namespace HPMSdk;

class HansoftUtils
{
private:
	/*********************************************
	* Creates an entry that can be commited to a sprint
	* Params:
	*	taskRefID - the ref id of the item to create an entry for (should be a backlog item)
	*   index - the local index of the task in the current list of items to add
	*   previousItem - the previous item to add the current one after
	*   parent - a reference to the parent
	* Returns:
	*	A entry that can be used to commit an item to a sprint
	*********************************************/
	static HPMTaskCreateUnifiedEntry createCommitEntry(HPMUniqueID taskRefID, unsigned index, HPMTaskCreateUnifiedReference &previousItem, HPMTaskCreateUnifiedReference &parent);

	/*********************************************
	* Searches through the comments of a task and finds the comment with the highest ID.
	* Params:
	*	taskID - the task to find the comment for
	* Returns:
	*   the index of the latest comment
	*********************************************/
	static HPMInt32 findGreatestCommentPostID(HPMUniqueID taskID);

	/*********************************************
	* Adds the specified items and any potential children to a list of cases to commit
	* Params:
	*	taskRefID - the ref id of the item to create an entry for
	*   index - the local index of the task in the current list of backlog items to addto add
	*   previousItem - the previous item to add the current one after
	*   parent - a reference to the parent
	* Returns:
	*	A list of entries that can be used to commit a number of items to a sprint
	*********************************************/
	static vector<HPMTaskCreateUnifiedEntry> createCommitEntryList(HPMUniqueID taskRefID, unsigned &index, HPMTaskCreateUnifiedReference &previousItem, HPMTaskCreateUnifiedReference parent);

	/*********************************************
	* Checks if a parent (at any level above)  is in the list of selected tasks
	* Params:
	*	taskRefID - the ref id to check for parents.
	*   selectedTasks - the lsit of selected tasks
	* Returns:
	*	true if a parent exists in the list
	*********************************************/
	static bool isParentIsList(HPMUniqueID taskRefID, vector<HPMUniqueID> tasks);

public:

	/*********************************************
	* Creates a new sprint and returns the id of the newly created sprint.
	* Params:
	*	name - the name of the new sprint
	*   duration - the number of days the cycle should last
	*   projectUID - the id of the project to add the cycle in
	* Returns:
	*	The reference ID of the created sprint.
	*********************************************/
	static HPMUniqueID createSprint(HPMString name, int duration, HPMUniqueID projectUID);


	/*********************************************
	* Commits the list of backlog items to the sprint. Will handle if both parent and children exists in the list
	* by committing the parent with all its children.
	* Params:
	*	sprintRefID - the sprint to commit the tasks to
	*   backlogItems - the list of backlog item ID's to commit
	*********************************************/
	static void commitBacklogItems(HPMUniqueID sprintRefID, HPMUniqueID projectUID, vector<HPMUniqueID> backlogItems);


	/*********************************************
	* Resets the workflow status and returns the item to the backlog.
	* Params:
	*	taskRefID - the ref id delete and reset.
	*********************************************/
	static void resetAndDeleteTask(HPMUniqueID taskRefID);


	/*********************************************
	* Creates a link between the two incoming items.
	* Params:
	*	fromRef - starting point
	*	toRef - end point
	*********************************************/
	static void createLink(HPMUniqueID fromRef, HPMUniqueID toRef);


	/*********************************************
	* Creates a new bug.
	* Params:
	*	name - the name of the new bug
	*	projectUID - project to create the bug in
	* Returns:
	*   the id of the new bug
	*********************************************/
	static HPMUniqueID createBug(HPMString name, HPMUniqueID projectUID);

	/*********************************************
	* Posts a comment at the top of the list of comments for the incoming task.
	* Params:
	*	taskRefID - the task to comment at
	*	commentStr - the comment to post
	*********************************************/
	static void postComment(HPMUniqueID taskRefID, HPMString commentStr);

	/*********************************************
	* The projects in the database that the SessionManager is connected to.
	* Params:
	*	projectName - name of the project to look for
	* Returns:
	*  the ID of the project with the corresponding name. -1 if it doesn't exist.
	**********************************************/
	static HPMUniqueID findProject(HPMString projectName);


	/*********************************************
	* Creates a backlog item in the project with the specified name, under the parent object (if not empty string).
	* The items will be called the item name.
	* Params:
	*	projectName - name of the project to create the item in the backlog
	*	parentName - name of the parent item to put the item under
	*	itemName - name for the newly created item
	* Returns:
	*  the ID of the newly created item. -1 if not successful.
	**********************************************/
	static HPMUniqueID createBacklogItem(HPMString projectName, HPMString parentName, HPMString itemName);
};

