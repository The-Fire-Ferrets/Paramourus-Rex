#ifndef COLLECTORCOMPONENT_H
#define COLLECTORCOMPONENT_H

#include "Constants.h"
#include "ActorComponent.h"
#include "ActorFactory.h"
#include "CollectableComponent.h"

class CollectorComponent : public ActorComponent {

	public:
		CollectorComponent();
		bool Init(pugi::xml_node* elem) override;
		void PostInit(void) override;

		void update(float time) override;
		void update(EventInterfacePtr e) override;
		void reset(void) override;
		void restart(void) override;
		void quit(void) override;

		void setVases(int v);
		int getVases(void);
		
		static ComponentId id;
		ComponentId getId(void) override;
		
	private:
		static ActorComponent* create();
		int vases;
		int flowers;
		static int instances;
		static const bool registered;
		StrongActorPtr* flower_list[];
};

#endif
