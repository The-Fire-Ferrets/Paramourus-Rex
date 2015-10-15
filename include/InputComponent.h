#ifndef INPUTCOMPONENT_H
#define INPUTCOMPONENT_H

#include "Constants.h"
#include "ActorComponent.h"
#include "ActorFactory.h"

// the four cardinal directions and a convenient list
const sf::Vector2f NORTH = sf::Vector2f(0.f, -1.f);
const sf::Vector2f SOUTH = sf::Vector2f(0.f, 1.f);
const sf::Vector2f EAST = sf::Vector2f(1.f, 0.f);
const sf::Vector2f WEST = sf::Vector2f(-1.f, 0.f);
const sf::Vector2f cardinals[4] = { NORTH, SOUTH, EAST, WEST };

class InputComponent : public ActorComponent {

    public:
        InputComponent();
        bool Init(pugi::xml_node* elem) override;
        void PostInit(void) override;

        void update(float time) override;
        void update(EventInterfacePtr e) override;
        void reset(void) override;
        void restart(void) override;
        void quit(void) override;

        static ComponentId id;
        ComponentId getId(void) override;

        sf::Vector2f getDirection() const;
        void setDirection(const sf::Vector2f& dir);

    private:
        float speed;
        static ActorComponent* create();
        static int instances;
        static const bool registered;

        sf::Vector2f direction;
};
#endif
