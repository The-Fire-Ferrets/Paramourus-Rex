#include "CraftEvent.h"

//Gets a reference to the event's memory location and uses it as a unique event id
EventType CraftEvent::event_type = EventType(&(CraftEvent::event_type));

/** Consturctor
 ** timeStamp: the event timeStamp
 ** a: a reference to the actor
 **/
CraftEvent::CraftEvent(const float timeStamp, const ActorInstance from, const ActorInstance to)  :  BaseEvent(timeStamp, from, to) {

}

/*CraftEvent::CraftEvent(std::istream& in) : BaseEvent(in) {

  }*/


/** Get the unique event type
 **
 **/
const EventType& CraftEvent::getEventType(void) const {
    return event_type;
}

/** Serialize event data
 **
 **/
void CraftEvent::Serialize(const std::ostream& o) const {

}

/** Get a copy of the event
 **
 **/
EventInterface* CraftEvent::copy(void) {
    return this;
}		
const EventInterface* CraftEvent::copy(void) const {
    return this;
}
