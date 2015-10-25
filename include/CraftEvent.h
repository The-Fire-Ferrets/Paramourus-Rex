#ifndef CRAFTEVENT_H
#define CRAFTEVENT_H

#include "Constants.h"
#include "BaseEvent.h"

class CraftEvent : public BaseEvent {
    public:
        static EventType event_type;

        explicit CraftEvent(const float timeStamp, const ActorInstance from, const ActorInstance to);
        //explicit CollectEvent(std::istream& in);		

        const EventType& getEventType(void) const override;
        void Serialize(const std::ostream& o) const override;
        EventInterface* copy(void) override;
        const EventInterface* copy(void) const override;
};

#endif
