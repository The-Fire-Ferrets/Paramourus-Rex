#ifndef CONTACTEVENT_H
#define CONTACTEVENT_H

#include "Constants.h"
#include "BaseEvent.h"

class ContactEvent : public BaseEvent {
	public:
		static EventType event_type;

		explicit ContactEvent(const float timeStamp, const ActorInstance from, const ActorInstance to);	

		const EventType& getEventType(void) const override;
		void Serialize(const std::ostream& o) const override;
		EventInterface* copy(void) override;
		const EventInterface* copy(void) const override;
};

#endif
