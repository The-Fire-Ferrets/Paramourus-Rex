#ifndef EVENTMANAGERINTERFACE_H
#define EVENTMANAGERINTERFACE_H

#include "Constants.h"
#include "EventInterface.h"
class EventManagerInterface {

public:
	static EventManagerInterfacePtr globalEventManager;

	explicit EventManagerInterface(void) {};
	virtual ~EventManagerInterface(void) {};

	virtual bool addDelegate(const EventDelegate& d, const EventType& type) = 0;

	virtual bool removeDelegate(const EventDelegate& d, const EventType& type) = 0;

	virtual bool triggerEvent(const EventInterfacePtr& event) = 0;
	
	virtual bool queueEvent(const EventInterface* event)  = 0;	

	virtual bool processEvents(void) = 0;

	static EventManagerInterfacePtr get(void);
	static void set(EventManagerInterfacePtr manager);
};
#endif



