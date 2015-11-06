#ifndef EVENTMANAGERINTERFACE_H
#define EVENTMANAGERINTERFACE_H

#include "Constants.h"
#include "EventInterface.h"
#include "Actor.h"
class EventManagerInterface {

	public:
		static EventManagerInterfacePtr globalEventManager;

		explicit EventManagerInterface(void) {};
		virtual ~EventManagerInterface(void) {};

		virtual bool addDelegate(const EventDelegate& d, const EventInstance& type) = 0;
		virtual bool removeDelegate(const EventDelegate& d) = 0;

		virtual bool triggerEvent(const EventInterfacePtr& event) = 0;

		virtual bool queueEvent(const EventInterface* event)  = 0;	

		virtual bool processEvents(void) = 0;

		virtual void quit(void) {};
		virtual void reset(void) {};

		static EventManagerInterfacePtr get(void);
		static void set(EventManagerInterfacePtr manager);

		static void setViewDelegate(const EventDelegate& d);
		static void setCurrentActorList(std::vector<StrongActorPtr>* aList);
		static std::vector<StrongActorPtr> getCurrentActorList(void);
		static StrongActorPtr getActor(int instance);

	protected:
		static EventDelegate currentViewDelegate;
		static std::vector<StrongActorPtr>* currentActorList;
};
#endif



