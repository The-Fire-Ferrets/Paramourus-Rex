#include "CollectEvent.h"

//Gets a reference to the event's memory location and uses it as a unique event id
EventType CollectEvent::event_type = EventType(&(CollectEvent::event_type));

/** Consturctor
 ** timeStamp: the event timeStamp
 ** a: a reference to the actor
 **/
CollectEvent::CollectEvent(const float timeStamp, const ActorInstance from, const ActorInstance to)  :  BaseEvent(timeStamp, from, to) {

}

/*CollectEvent::CollectEvent(std::istream& in) : BaseEvent(in) {

  }*/


/** Get the unique event type
 **
 **/
const EventType& CollectEvent::getEventType(void) const {
    return event_type;
}

/** Serialize event data
 **
 **/
void CollectEvent::Serialize(const std::ostream& o) const {

}

/** Get a copy of the event
 **
 **/
EventInterface* CollectEvent::copy(void) {
    return this;
}		
const EventInterface* CollectEvent::copy(void) const {
    return this;
}
