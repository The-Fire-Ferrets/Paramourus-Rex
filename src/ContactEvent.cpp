#include "ContactEvent.h"

//Gets a reference to the event's memory location and uses it as a unique event id
EventType ContactEvent::event_type = EventType(&(ContactEvent::event_type));

/** Consturctor
 ** timeStamp: the event timeStamp
 ** a: a reference to the actor
**/
ContactEvent::ContactEvent(const float timeStamp, StrongActorPtr& a)  :  BaseEvent(timeStamp) {
	actor = a;
}

/*ContactEvent::ContactEvent(std::istream& in) : BaseEvent(in) {

}*/

/** Get the pointer to the actor made contact with
 **
**/
StrongActorPtr ContactEvent::getActor(void) const {
	return actor;
}

/** Get the unique event type
 **
**/
const EventType& ContactEvent::getEventType(void) const {
	return event_type;
}

/** Get the event time stamp
 **
**/
float ContactEvent::GetTimeStamp(void) const {
	return time_stamp;
}

/** Serialize event data
 **
**/
void ContactEvent::Serialize(const std::ostream& o) const {

}

/** Get a copy of the event
 **
**/
EventInterface* ContactEvent::copy(void) {
	return this;
}		
const EventInterface* ContactEvent::copy(void) const {
	return this;
}
