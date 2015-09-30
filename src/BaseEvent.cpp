#include "BaseEvent.h"

/** Constructor
 **
**/  
BaseEvent::BaseEvent(const float timeStamp = 0.0f)  : time_stamp(timeStamp) {

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
