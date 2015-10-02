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
		void PostInit(void) override;

		static ComponentId id;
		
		void update(float time) override;	
		void reset(void) override;
		void restart(void) override;
		void quit(void) override;

		ComponentId getId(void) override;

	private:
		static ActorComponent* create();
		static const bool registered;
		static int instances;
};

#endif
