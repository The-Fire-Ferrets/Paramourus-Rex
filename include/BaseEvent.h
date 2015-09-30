#ifndef BASEEVENT_H
#define BASEEVENT_H

#include "Constants.h"
#include "EventInterface.h"

class BaseEvent : public EventInterface {
	public:
		const float time_stamp;

		explicit BaseEvent(const float timeStamp);
		//explicit BaseEvent(std::istream& in);
		virtual ~BaseEvent(void);
		
		virtual const EventType& getEventType(void) const override = 0;
		float GetTimeStamp(void) const override;
		virtual void Serialize(const std::ostream& o);
		virtual const EventInterface* copy(void) const = 0;
};
#endif
