#ifndef EVENTINTERFACE_H
#define EVENTINTERFACE_H

#include "Constants.h"

class EventInterface {
	public:
		virtual const EventType& getEventType(void) const = 0;
		virtual float getTimeStamp(void) const = 0;
		virtual void Serialize(const std::ostream& o) const = 0;
		virtual EventInterface* copy(void) = 0;
		virtual const EventInterface* copy(void) const = 0;
		virtual const ActorInstance getSender(void) const = 0;
		virtual const ActorInstance getReceiver(void) const = 0;
};
#endif
