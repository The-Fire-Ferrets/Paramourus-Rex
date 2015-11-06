#ifndef DIALOGUEVIEW_H
#define DIALOGUEVIEW_H

#include "Constants.h"
#include "ActorFactory.h"
#include <vector>

class DialogueView {

	private:
		static std::string name;
		static int numDialogues;
		static const int size;
		static int index;
		static std::string dialogue_str;
		
		//images
		static sf::Texture background_texture;
		static sf::Sprite Diana;
		static sf::Sprite Phil;
		static sf::Sprite background;
		static sf::RectangleShape backlay;

		// text values
		static sf::Font font;
		static sf::Text text;

		// positions
		static sf::Vector2f dianaPos;
		static sf::Vector2f philPos;
		static sf::Vector2f dialogue_pos;

		static bool pressed;

		static std::vector<std::string> fitStringToDialogueBox(std::string str);

		static sf::SoundBuffer buffer;
		static sf::Sound sound;
		static int view_state;

	public:
		static std::vector<std::string> boxes;     // what gets displayed on screen
		static std::vector<std::string> dialogues; // what gets read in from the XML
		static void Create(const char* resource, int* state);
		static void update(sf::RenderWindow *window, int* state);
		static void update(EventInterfacePtr e);
		static void render(sf::RenderWindow *window);
		static void cleanUp();
};

#endif
