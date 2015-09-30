#include "EventManagerInterface.h"

//Declares the global event manager
EventManagerInterfacePtr EventManagerInterface::globalEventManager = NULL;

//Gets a reference to the global event manager
EventManagerInterfacePtr EventManagerInterface::get(void) {
	return globalEventManager;

}

//Sets the global event manager
void EventManagerInterface::set(EventManagerInterfacePtr manager) {
	globalEventManager = manager;
}
