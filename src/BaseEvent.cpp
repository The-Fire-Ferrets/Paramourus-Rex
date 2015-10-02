#include "BaseEvent.h"

/** Constructor
 **
**/  
BaseEvent::BaseEvent(const float timeStamp = 0.0f, const ActorInstance from = -1, const ActorInstance to = -1)  : time_stamp(timeStamp), sender(from), receiver(to) {


}

/*BaseEvent::BaseEvent(std::istream& in) {
	time_stamp = 0.0f;
}*/

/** Destructor
 **
**/ 
BaseEvent::~BaseEvent(void) {

}

/** Serialize the event data
 **
**/ 
void BaseEvent::Serialize(const std::ostream& o) {

}

/** Get the event time stamp
 **
**/ 
float BaseEvent::GetTimeStamp(void) const {
	return time_stamp;
}

/** Return the instance of the sender
 **
**/
const ActorInstance BaseEvent::getSender(void) const {
	return sender;
}

/** return the isntance of the receiver
 **
**/
const ActorInstance BaseEvent::getReceiver(void) const {
	return receiver;
}