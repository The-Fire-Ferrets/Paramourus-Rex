#ifndef ACTORFACTORY_H
#define ACTORFACTORY_H

#include "Constants.h"
#include "Actor.h"
#include "ActorComponent.h"

class ActorFactory {
	private:
		static const int size;
	public:
		static StrongActorPtr CreateActor(const char* resource, int* state);
		static bool registerComponent(ComponentId* id, ActorComponentCreator creator);
		static void reset(void);

	protected:
		static std::vector<ComponentId> componentCreatorList;
		static ComponentId* actorComponentIds[];
		static ActorComponentCreator actorComponentCreators[];
		static StrongActorComponentPtr CreateComponent(pugi::xml_node* elem);
		static StrongActorComponentPtr CreateComponent(const pugi::xml_node* elem);
};

#endif
