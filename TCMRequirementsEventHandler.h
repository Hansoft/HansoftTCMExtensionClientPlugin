#include "../HansoftSDK/HPMSdkCpp.h"
#include "TCMEventHandler.h"

using namespace std;
using namespace HPMSdk;

class CTCMRequirementsEventHandler : public CTCMEventHandler
{
private:
	const HPMString s_requirementsProjectName = hpm_str("Requirements");

	const HPMString s_developmentProjectName = hpm_str("Development");
	const HPMString s_developmentRootName = hpm_str("Root");

	const HPMString s_testCasesProjectName = hpm_str("Test Cases");
	const HPMString s_testSuitesRootName = hpm_str("Test suites");

	const HPMString s_addSuiteMenuAlternative = hpm_str("Add Test Suite");
	const HPMString s_addSuiteItem = hpm_str(".taskmenu.tcmclientplugin.addsuite");

	const HPMString s_addUserStoryAlternative = hpm_str("Add User Story");
	const HPMString s_addUserStoryItem = hpm_str(".taskmenu.tcmclientplugin.adduserstory");


	const HPMString s_addUserStoryDialog = hpm_str("com.hansoft.tcm.clientplugin.adduserstorydialog");
	const HPMString s_addUserStoryDialogUserStory = hpm_str("/Form/UserStory");

	const HPMString s_addTestSuiteDialog = hpm_str("com.hansoft.tcm.clientplugin.addtestsuitedialog");
	const HPMString s_addTestSuiteDialogTestSuite = hpm_str("/Form/TestSuite");

	HPMString m_addUserStoryDialogue;
	HPMString m_userStory;

	HPMString m_addTestSuiteDialogue;
	HPMString m_testSuite;

	/*********************************************
	* Called when the user invokes the add test suite alternative
	*********************************************/
	void onCreateTestSuite(HPMString testSuiteName, std::vector<HPMUniqueID> selectedTasks, HPMUniqueID projectUID);

	/*********************************************
	* Called when the user invokes the add user story alternative.
	*********************************************/
	void onCreateUserStory(HPMString userStory, std::vector<HPMUniqueID> selectedTasks, HPMUniqueID projectUID);
public:

	CTCMRequirementsEventHandler(HPMSdkSession *_pSession);

	/*********************************************
	* If the inheriting event handler is interested in the action it should perform any actions it would like to do and
	* return true.
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

