#ifndef DIALOGUEVIEW_H
#define DIALOGUEVIEW_H

#include "Constants.h"
#include "ActorFactory.h"

class DialogueView {

	private:
		static int numDialogues;
		static const int size;
		static int index;
		static std::string dialogue_str;
		
		//images
		static sf::Texture background_texture;
		static sf::Sprite Diana;
		static sf::Sprite Phil;
		static sf::Sprite background;
		
		// text values
		static sf::Font font;
		static sf::Text text;
		
		// positions
		static sf::Vector2f dianaPos;
		static sf::Vector2f philPos;
		static sf::Vector2f dialogue_pos;

	public:
		static std::string boxes[];
		static void Create(const char* resource, int* state);
		static void update(sf::RenderWindow *window, int* state);
		static void update(EventInterfacePtr e);
		static void render(sf::RenderWindow *window);
};

#endif
