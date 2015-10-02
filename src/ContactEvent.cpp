#include "ContactEvent.h"

//Gets a reference to the event's memory location and uses it as a unique event id
EventType ContactEvent::event_type = EventType(&(ContactEvent::event_type));

/** Consturctor
 ** timeStamp: the event timeStamp
 ** a: a reference to the actor
**/
ContactEvent::ContactEvent(const float timeStamp, const ActorInstance from, const ActorInstance to)  :  BaseEvent(timeStamp, from, to) {

}

/*ContactEvent::ContactEvent(std::istream& in) : BaseEvent(in) {

}*/


/** Get the unique event type
 **
**/
const EventType& ContactEvent::getEventType(void) const {
	return event_type;
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
