#ifndef COLLECTEVENT_H
#define COLLECTEVENT_H

#include "Constants.h"
#include "BaseEvent.h"

class CollectEvent : public BaseEvent {
    public:
        static EventType event_type;

        explicit CollectEvent(const float timeStamp, const ActorInstance from, const ActorInstance to);
        //explicit CollectEvent(std::istream& in);		

        const EventType& getEventType(void) const override;
        void Serialize(const std::ostream& o) const override;
        EventInterface* copy(void) override;
        const EventInterface* copy(void) const override;
};

#endif
