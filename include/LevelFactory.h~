#ifndef ACTORFACTORY_H
#define ACTORFACTORY_H

#include "Constants.h"

class ActorFactory {
	private:
		static const int size;
		static int num_actors;
		static std::string background;
		static std::string name;
	public:
		static StrongActorPtr actors[];
		static void CreateLevel(const char* resource, int* state);
		
		static std::string getName(void);
		static int getNumActors(void);
};

#endif
