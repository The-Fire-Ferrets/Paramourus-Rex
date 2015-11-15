#ifndef INPUTCOMPONENT_H
#define INPUTCOMPONENT_H

#include "Constants.h"
#include "ActorComponent.h"
#include "ActorFactory.h"

// the four cardinal directions and a convenient list
const sf::Vector2f NORTH = sf::Vector2f(0.f, -1.f);
const sf::Vector2f NORTHWEST = sf::Vector2f(-1.f, -1.f);
const sf::Vector2f NORTHEAST = sf::Vector2f(1.f, -1.f);
const sf::Vector2f SOUTH = sf::Vector2f(0.f, 1.f);
const sf::Vector2f SOUTHWEST = sf::Vector2f(-1.f, 1.f);
const sf::Vector2f SOUTHEAST = sf::Vector2f(1.f, 1.f);
const sf::Vector2f EAST = sf::Vector2f(1.f, 0.f);
const sf::Vector2f WEST = sf::Vector2f(-1.f, 0.f);
const sf::Vector2f cardinals[4] = { NORTH, SOUTH, EAST, WEST };

class InputComponent : public ActorComponent {

	public:
		InputComponent();
		~InputComponent();

		bool Init(pugi::xml_node* elem) override;
		bool PostInit(pugi::xml_node* elem) override;
		bool PostInit(void) override;

		void update(float time) override;
		void update(EventInterfacePtr e) override;
		void render(sf::RenderWindow *window, bool minimap) override;
		void reset(void) override;
		void restart(void) override;
		void quit(void) override;

		static ComponentId id;
		ComponentId getId(void) const override;

		sf::Vector2f getDirection() const;
		void setDirection(const sf::Vector2f& dir);

	private:
		float speed;
		int last_framerate;
		static ActorComponent* create();
		static int instances;
		static const bool registered;
		sf::Vector2f direction;
		bool first_postinit;
		sf::Vector2f fake_pos;
		sf::Vector2f fake_dir;
};
#endif
