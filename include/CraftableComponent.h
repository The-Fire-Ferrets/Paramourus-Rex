#ifndef CRAFTABLECOMPONENT_H
#define CRAFTABLECOMPONENT_H

#include "Constants.h"
#include "ActorComponent.h"
#include "ActorFactory.h"

class CraftableComponent : public ActorComponent {

    public:
        CraftableComponent();

        bool Init(pugi::xml_node* elem) override;
        void PostInit(void) override;

        void update(float time) override;
		void update(EventInterfacePtr e) override;
		void render(sf::RenderWindow *window) override;
        void reset(void) override;
        void restart(void) override;
        void quit(void) override;

        static ComponentId id;
        ComponentId getId(void) override;

    private:
        static ActorComponent* create();
        static int instances;
        static const bool registered;
};

#endif
