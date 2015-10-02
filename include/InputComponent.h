#ifndef INPUTCOMPONENT_H
#define INPUTCOMPONENT_H

#include "Constants.h"
#include "ActorComponent.h"
#include "ActorFactory.h"

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

    // the four cardinal directions and a convenient list
    const sf::Vector2f NORTH;
    const sf::Vector2f SOUTH;
    const sf::Vector2f EAST;
    const sf::Vector2f WEST;
    const sf::Vector2f cardinals[4];

	private:
		static ActorComponent* create();
		static int instances;
		static const bool registered;
    static constexpr float velocity = 50.f;

    sf::Vector2f direction;
};
#endif
