#ifndef ACTORFACTORY_H
#define ACTORFACTORY_H

#include "Constants.h"
#include "Actor.h"
#include "ActorComponent.h"

class ActorFactory {
	private:
		static ActorId nextActorId(void);
		static const int size;
	public:
		static ActorId currActorId;
		static StrongActorPtr CreateActor(const char* resource, int* state);
		static bool registerComponent(ComponentId* id, ActorComponentCreator creator);
		static StrongActorPtr actorInstances[size];

	protected:
		static ComponentId* actorComponentIds[size];
		static ActorComponentCreator actorComponentCreators[size];
		static StrongActorComponentPtr CreateComponent(pugi::xml_node* elem);
};

#endif
