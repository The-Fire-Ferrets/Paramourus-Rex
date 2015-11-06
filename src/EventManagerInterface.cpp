#include "EventManagerInterface.h"

//Declares the global event manager
EventManagerInterfacePtr EventManagerInterface::globalEventManager = NULL;
EventDelegate EventManagerInterface::currentViewDelegate = NULL;
std::vector<StrongActorPtr>* EventManagerInterface::currentActorList = NULL;
//Gets a reference to the global event manager
EventManagerInterfacePtr EventManagerInterface::get(void) {
    return globalEventManager;

}

//Sets the global event manager
void EventManagerInterface::set(EventManagerInterfacePtr manager) {
    globalEventManager = manager;
}

void EventManagerInterface::setViewDelegate(const EventDelegate& d) {
	currentViewDelegate = d;
}

void EventManagerInterface::setCurrentActorList(std::vector<StrongActorPtr>* aList) {
	currentActorList = aList;
}

std::vector<StrongActorPtr> EventManagerInterface::getCurrentActorList(void) {
	return *currentActorList;
}

StrongActorPtr EventManagerInterface::getActor(int instance) {
	if (currentActorList != NULL) {
		std::vector<StrongActorPtr>::iterator it;
		for (it = (*currentActorList).begin(); it != (*currentActorList).end(); it++) {
			if ((*it)->getInstance() == instance) {
				return *it;
			}
		}
	}
	return NULL;
}
