#ifndef BASEEVENT_H
#define BASEEVENT_H

#include "Constants.h"
#include "EventInterface.h"

class BaseEvent : public EventInterface {
	public:
		explicit BaseEvent(const float timeStamp, const ActorInstance from, const ActorInstance to);
		//explicit BaseEvent(std::istream& in);
		virtual ~BaseEvent(void);
		
		virtual const EventType& getEventType(void) const override = 0;
		float GetTimeStamp(void) const override;
		virtual void Serialize(const std::ostream& o);
<<<<<<< HEAD
		virtual const EventInterface* copy(void) const = 0;

		const ActorInstance getSender(void) const override;
		const ActorInstance getReceiver(void) const override;
	protected:
		const float time_stamp;
		const ActorInstance sender;
		const ActorInstance receiver;
=======
		virtual const EventInterface* copy(void) const override = 0;
>>>>>>> a872fa7265274f4af690e933e47a6ccfe4033d91
};
#endif
