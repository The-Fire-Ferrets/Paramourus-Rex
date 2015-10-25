#ifndef COLLECTABLECOMPONENT_H
#define COLLECTABLECOMPONENT_H

#include "Constants.h"
#include "ActorComponent.h"
#include "AI.h"
#include "ActorFactory.h"
#include "CollectorComponent.h"
#include "ContactEvent.h"

class CollectableComponent : public ActorComponent {
	friend class AI;
	public:
		CollectableComponent();

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
		StrongActorPtr collector;
};

#endif
