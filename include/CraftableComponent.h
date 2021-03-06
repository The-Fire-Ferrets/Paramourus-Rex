#ifndef CRAFTABLECOMPONENT_H
#define CRAFTABLECOMPONENT_H

#include "Constants.h"
#include "ActorComponent.h"
#include "ActorFactory.h"

#include <vector>
#include <string>

class CraftableComponent : public ActorComponent {

	public:
		CraftableComponent();
		~CraftableComponent();

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

		bool doesCombineWith(const CraftableComponent& other) const;
		void combineWith(const CraftableComponent& other);

	private:
		static ActorComponent* create();
		static int instances;
		static const bool registered;
		std::vector<std::string> elements; // Long ago, the four nations lived together in harmony

		std::string getCraftResultValue(pugi::xml_node node, std::string name);
};

#endif
