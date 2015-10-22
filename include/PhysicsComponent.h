#ifndef PHYSICSCOMPONENT_H
#define PHYSICSCOMPONENT_H

#include "Constants.h"
#include "Actor.h"
#include "ActorComponent.h"
#include "ActorFactory.h"
#include "EventManagerInterface.h"
#include "ContactEvent.h"
#include "LevelView.h"

class PhysicsComponent : public ActorComponent {

    public:
        PhysicsComponent(void);
        ~PhysicsComponent(void);
        bool Init(pugi::xml_node* elem) override;
        void PostInit(void) override;

        static ComponentId id;

        void update(float time) override;	
        void update(EventInterfacePtr e) override;
	void render(sf::RenderWindow *window) override;
        void reset(void) override;
        void restart(void) override;
        void quit(void) override;
        bool query(sf::FloatRect bound, sf::Vector2f dir);
	std::vector<StrongActorPtr> last_actors;
        ComponentId getId(void) override;

    private:
        static ActorComponent* create();
        static const bool registered;
        static int instances;
        sf::Vector2f last_dir;
};

#endif
