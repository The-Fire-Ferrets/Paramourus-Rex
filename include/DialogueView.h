#ifndef DIALOGUEVIEW_H
#define DIALOGUEVIEW_H

#include "Constants.h"
#include "ActorFactory.h"

class DialogueView {

	private:
		static int maxDialogues;
		static int index;
		static std::string background;
		static std::string level;
		static std::string text;

	public:
		static std::string boxes[];
		void update(const char* resource, int* state);
		void changeState(int* state);
		std::string getDialogue(const char* resource, int* state);
};

#endif
