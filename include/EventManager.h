#ifndef EVENTMANAGER_H
#define EVENTMANAGER_H

#include "Constants.h"
#include "EventManagerInterface.h"
#include "EventInterface.h"

class EventManager : public EventManagerInterface {
	public:
		explicit EventManager(void);
		virtual ~EventManager(void);

		EventQueue queues[2];
		EventQueuePtr process_queue = std::make_shared<EventQueue>(queues[0]);

		EventQueuePtr register_queue = std::make_shared<EventQueue>(queues[1]);

		EventDelegateMap eventDelegateMap;

		bool queueEvent(const EventInterface* event) override;
		bool addDelegate(const EventDelegate& d, const EventInstance& type) override;
		bool removeDelegate(const EventDelegate& d) override;
		bool triggerEvent(const EventInterfacePtr& event) override;
		bool processEvents(void) override;
		void quit(void) override;
		void reset(void) override;
};
#endif



