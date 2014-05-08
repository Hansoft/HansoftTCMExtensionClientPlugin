#ifndef __CTCTMEVENTHANDLER_H
#define __CTCTMEVENTHANDLER_H

#include "../HansoftSDK/HPMSdkCpp.h"
#include <tchar.h>
#include <conio.h>
#include <windows.h>

#include <iostream>
#include <string>
#include <sstream>
#include <iterator>
#include <memory>

using namespace std;
using namespace HPMSdk;

class CTCMEventHandler
{
protected:
	HPMSdkSession *m_pSession;

public: 

	// Simple enum to keep track of the sections in a Hansoft project
	enum HansoftSection
	{
		BACKLOG = 0,
		SCHEDULE = 1,
		QA =2
	};

	CTCMEventHandler(HPMSdkSession *_pSession)
	{
		m_pSession = _pSession;
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
	virtual bool handleThis(HPMString action, std::vector<HPMUniqueID> &selectedTasks, HPMUniqueID projectUID) { return false; };

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
	virtual bool addYourStuffToRightClick(HPMRightClickContext rightClickContext, HPMString projectName, HPMString root, HansoftSection section, std::vector<HPMUniqueID> &selectedTasks)  { return false; };
};

#endif //__CTCTMEVENTHANDLER_H
