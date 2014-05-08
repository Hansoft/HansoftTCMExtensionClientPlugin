#include "../HansoftSDK/HPMSdkCpp.h"
#include "TCMEventHandler.h"

using namespace std;
using namespace HPMSdk;

class CTCMTestCycleEventHandler : public CTCMEventHandler
{
private:
	const HPMString s_testCaseProjectName = hpm_str("Test Cases");
	const HPMString s_startCycleMenuAlternative = hpm_str("Start Test Cycle");
	const HPMString s_startCycleItem = hpm_str(".taskmenu.tcmclientplugin.startcycle");

	const HPMString s_endCycleMenuAlternative = hpm_str("End Test Cycle");
	const HPMString s_endCycleItem = hpm_str(".taskmenu.tcmclientplugin.endcycle");

	const HPMString s_reportBugMenuAlternative = hpm_str("Report Bug");
	const HPMString s_reportBugItem = hpm_str(".taskmenu.tcmclientplugin.reportbug");

	/*********************************************
	* Creates a new test cycle to commit selected test cases to
	* Params:
	*	name - the name of the new test cycle
	*   duration - the number of days the cycle should lasta
	*   projectUID - the id of the project to add the cycle in
	* Returns:
	*	The reference ID of the created sprint.
	*********************************************/
	HPMUniqueID createTestCycle(HPMString name, int duration, HPMUniqueID projectUID);

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
	HPMTaskCreateUnifiedEntry createEntryForTestCase(HPMUniqueID taskRefID, unsigned index, HPMTaskCreateUnifiedReference &previousTestCase, HPMTaskCreateUnifiedReference &parent);

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
	std::vector<HPMTaskCreateUnifiedEntry> createTestCaseEntryList(HPMUniqueID taskRefID, unsigned &index, HPMTaskCreateUnifiedReference &previousTestCase, HPMTaskCreateUnifiedReference parent);

	/*********************************************
	* Checks if a parent (at any level above)  is in the list of selected tasks
	* Params:
	*	taskRefID - the ref id to check for parents.
	*   selectedTasks - the lsit of selected tasks
	* Returns:
	*	true if a parent exists in the list
	*********************************************/
	bool isParentIsList(HPMUniqueID taskRefID, std::vector<HPMUniqueID> selectedTasks);

	/*********************************************
	* Called when the user has selected a number of items
	* in the backlog of the test case management project and
	* chosen to start a test cycle.
	*********************************************/
	void onStartTestCycle(std::vector<HPMUniqueID> selectedTasks, HPMUniqueID projectUID);

	/*********************************************
	* Called when the user invokes the end cycle menu alternative.
	*********************************************/
	void onEndTestCycle(std::vector<HPMUniqueID> selectedTasks, HPMUniqueID projectUID);

	/*********************************************
	* Called when the user invokes the report bug menu alternative.
	*********************************************/
	void onReportBug(std::vector<HPMUniqueID> selectedTasks, HPMUniqueID projectUID);

	/*********************************************
	* Resets the workflow status and returns the item to the backlog.
	* Params:
	*	taskRefID - the ref id delete and reset.
	*********************************************/
	void resetAndDeleteTask(HPMUniqueID taskRefID);

public:

	CTCMTestCycleEventHandler(HPMSdkSession *_pSession) : CTCMEventHandler(_pSession)
	{
		
	};

	/*********************************************
	* Handles the actions defined in s_startCycleItem and s_endCycleItem.
	* Params:
	*	action - the string defining the action that was invoked by the user.
	*   selectedTasks - the current selection of tasks
	*   projectUID - the unique identifier for the current project.
	* Returns:
	*	Returns true if the event handler will pick up this action
	*********************************************/
	virtual bool handleThis(HPMString action, std::vector<HPMUniqueID> &selectedTasks, HPMUniqueID projectUID);

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
	virtual bool addYourStuffToRightClick(HPMRightClickContext rightClickContext, HPMString projectName, HPMString root, HansoftSection section, std::vector<HPMUniqueID> &selectedTasks);
};

