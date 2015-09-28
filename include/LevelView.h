#ifndef LEVELVIEW_H
#define LEVELVIEW_H

#include "Constants.h"
#include "ActorFactory.h"

class LevelView {
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
