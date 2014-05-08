#include "../HansoftSDK/HPMSdkCpp.h"
#include "TCMEventHandler.h"

using namespace std;
using namespace HPMSdk;

class CTCMRequirementsEventHandler : public CTCMEventHandler
{
private:
	const HPMString s_requirementsProjectName = hpm_str("Requirements");

	const HPMString s_addSuiteMenuAlternative = hpm_str("Add Test Suite");
	const HPMString s_addSuiteItem = hpm_str(".taskmenu.tcmclientplugin.addsuite");

	const HPMString s_addUserStoryAlternative = hpm_str("Add User Story");
	const HPMString s_addUserStoryItem = hpm_str(".taskmenu.tcmclientplugin.adduserstory");

public:

	CTCMRequirementsEventHandler(HPMSdkSession *_pSession) : CTCMEventHandler(_pSession)
	{

	};

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

};

