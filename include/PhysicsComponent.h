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
        bool PostInit(pugi::xml_node* elem) override;

	void setDirectionBit(int bit_num);
	void setDirectionBit(sf::Vector2f dir);
	bool getDirectionBit(int bit_num);
	bool getDirectionBit(sf::Vector2f dir);
	void flipDirectionBit(int bit_num);
	void flipDirectionBit(sf::Vector2f dir);
	void resetDirectionBit();

        static ComponentId id;

        void update(float time) override;	
        void update(EventInterfacePtr e) override;
		void render(sf::RenderWindow *window, bool minimap) override;
        void reset(void) override;
        void restart(void) override;
        void quit(void) override;
        bool query(sf::FloatRect bound, sf::Vector2f dir);
		std::vector<StrongActorPtr> last_actors;
		std::vector<sf::FloatRect*> last_boundaries;
		ComponentId getId(void) const override;

    private:
        static ActorComponent* create();
        static const bool registered;
        static int instances;
	int direction_bit;
        sf::Vector2f last_dir;
};

#endif
