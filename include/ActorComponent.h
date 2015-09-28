#ifndef ACTORCOMPONENT_H 
#define ACTORCOMPONENT_H

#include "Constants.h"

class ActorComponent {

	friend class ActorFactory;
	
	protected:
		StrongActorPtr owner;
		int instance;
		std::string type;

	public:
		virtual ~ActorComponent(void) {};
		virtual bool Init(pugi::xml_node* elem) = 0;
		int getInstance(void);
		virtual void PostInit(void) {};

		virtual void update(float time) {};
		virtual void reset(void) {};
		virtual void restart(void) {};
		virtual void quit(void) {};
		virtual ComponentId getId(void) = 0;
		std::string getType(void);

	private:
		void SetOwner(StrongActorPtr p_owner);

};

#endif
