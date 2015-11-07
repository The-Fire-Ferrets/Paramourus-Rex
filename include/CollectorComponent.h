#ifndef COLLECTORCOMPONENT_H
#define COLLECTORCOMPONENT_H

#include "Constants.h"
#include "ActorComponent.h"
#include "ActorFactory.h"
#include "CollectableComponent.h"

class CollectorComponent : public ActorComponent {

	public:
		CollectorComponent();
		~CollectorComponent();

		bool Init(pugi::xml_node* elem) override;
		bool PostInit(pugi::xml_node* elem) override;
		bool PostInit(void) override;

		void update(float time) override;
		void update(EventInterfacePtr e) override;
		void render(sf::RenderWindow *window, bool minimap) override;
		void reset(void) override;
		void restart(void) override;
		void quit(void) override;

		void setVases(int v);
		int getVases(void);
		std::vector<StrongActorPtr> getFlowers(void);		
		static ComponentId id;
		ComponentId getId(void) const override;

	private:
		static ActorComponent* create();
		int vases;
		int vases_num;
		int vase_size;
		int flowers;
		static int instances;
		static const bool registered;
		std::vector<StrongActorPtr> flowerList;

		std::vector<sf::Sprite> vase_sprites;
		sf::Texture vase_empty;
		sf::Texture vase_fireflower;
		sf::Texture vase_earthflower;
		sf::Texture vase_waterflower;
		sf::Texture vase_airflower;
};

#endif
