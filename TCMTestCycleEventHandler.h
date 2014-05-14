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

	const HPMString s_reportBugDialog = hpm_str("com.hansoft.tcm.clientplugin.reportbugdialog");
	const HPMString s_reportBugDialogBugTitle = hpm_str("/Form/BugTitle");

	HPMString m_addBugDialogue;
	HPMString m_bugTitle;

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
	void onReportBug(HPMString bugName, std::vector<HPMUniqueID> selectedTasks, HPMUniqueID projectUID);

public:

	CTCMTestCycleEventHandler(HPMSdkSession *_pSession);

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

	/*********************************************
	* Called when a custom dialog data has changed
	* Params:
	*	dataID - the id of the data that has changed
	*	value - the new value for the data
	* Returns:
	*	Returns true if the event handler is interested in this data update
	*********************************************/
	virtual bool customDataUpdated(HPMString dataID, HPMString value);

	/*********************************************
	* Called when a custom dialog has exited with ok
	* Params:
	*	dialog - the dialog that has been closed
	*   selectedTasks - the current selection of tasks
	*   projectUID - the unique identifier for the current project.
	* Returns:
	*	Returns true if the event handler is interested in this data update
	*********************************************/
	virtual bool customDialogClosed(HPMString dialog, std::vector<HPMUniqueID> &selectedTasks, HPMUniqueID projectUID);
};

