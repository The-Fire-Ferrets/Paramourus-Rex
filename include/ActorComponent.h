#ifndef ACTORCOMPONENT_H 
#define ACTORCOMPONENT_H

#include "Constants.h"
#include "CollectEvent.h"
#include "AI.h"

class ActorComponent {
	friend class AI;
	friend class ActorFactory;

	protected:
	StrongActorPtr owner;
	int instance;
	std::string type;

	public:
		virtual ~ActorComponent(void) {};
		virtual bool Init(pugi::xml_node* elem) = 0;
		int getInstance(void);
		virtual bool PostInit(pugi::xml_node* elem) = 0;
		virtual bool PostInit(void) = 0;

		virtual void update(float time) {};
		virtual void update(EventInterfacePtr e) {};
		virtual void render(sf::RenderWindow *window, bool minimap) {};
		virtual void reset(void) {};
		virtual void restart(void) {};
		virtual void quit(void) {};
		virtual ComponentId getId(void) const = 0;
		std::string getType(void);

		bool operator==(const ActorComponent& rhs) const;

	private:
		void SetOwner(StrongActorPtr p_owner);
		bool initial_init = true;

};

#endif
