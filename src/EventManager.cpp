#include "EventManager.h"

/** Constructor
 **
 **/
EventManager::EventManager(void)  : EventManagerInterface(){

}

/** Destructor
 **
 **/
EventManager::~EventManager(void) {

}

/** Add the event to the queue
 ** event: event to be processed
 **/
bool EventManager::queueEvent(const EventInterface* event)  {
    register_queue->push_back(event);
    return true;
};

/** Add a listener to the map
 ** d: the function to be called when the event type is received
 ** type: the event type to listen for
 **/
bool EventManager::addDelegate(const EventDelegate& d, const EventInstance& type) {
    EventDelegateList& delegateList = eventDelegateMap[type];
    for (auto itr = delegateList.begin(); itr != delegateList.end(); ++itr) {
        if (d == (*itr)) {
            return false;
        }
    }
    delegateList.push_back(d);
    return true;
}

/** remove a listener to the map
 ** d: the function to be called when the event type is received
 ** type: the event type to listen for
 **/
bool EventManager::removeDelegate(const EventDelegate& d) {
    EventDelegateMap::iterator it;
    for (it = eventDelegateMap.begin(); it != eventDelegateMap.end(); it++) {
        EventDelegateList& delegateList = it->second;
        for (auto itr = delegateList.begin(); itr != delegateList.end(); ++itr) {			
            if (*itr == d) {
                delegateList.erase(itr);
                return true;
            }
        }
    }
    return false;
}

/** Call all function listening for the given event
 ** event: the event being listened for
 **/
bool EventManager::triggerEvent(const EventInterfacePtr& event) {
    const EventInstance& type = EventInstance(event->getEventType(), event->getReceiver());
    auto eventDelegatePair = eventDelegateMap.find(type);
    if (eventDelegatePair  != eventDelegateMap.end()) {
        EventDelegateList& delegateList = eventDelegatePair->second;
        for (auto itr = delegateList.begin(); itr != delegateList.end(); ++itr) {			
            (*itr)(event);
        }
        return true;
    }
    return false;
}

/**  process all events in the queue
 **
 **/
bool EventManager::processEvents(void) {
    // swap	
    std::swap(process_queue, register_queue);
    register_queue->clear();
    // process all
    while(!process_queue->empty()) {
        EventInterfacePtr event;
        event.reset(process_queue->front());
        process_queue->pop_front();
        if(!triggerEvent(event))
            return false;
    }
    return true;
}

/**  quit all events in the queue
 **
 **/
void EventManager::quit(void) {
    reset();
    //EventManagerInterface::set(NULL);	
}

/**  reset all events in the queue
 **
 **/
    void EventManager::reset(void) {
        while(!process_queue->empty())
            process_queue->pop_front();

        while(!register_queue->empty())
            register_queue->pop_front();
    }
