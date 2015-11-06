#ifndef TITLEVIEW_H
#define TITLEVIEW_H

#include "Constants.h"
#include "MapView.h"
#include "LevelView.h"

class TitleView {
	private:
		static sf::Sprite background;
		static sf::Texture background_texture;
		static std::string playbutton_string;
		static int playbutton_size;
		static sf::Text playbutton;
		static std::string introbutton_string;
		static int introbutton_size;
		static sf::Text introbutton;
		static std::string introdialogue;
		static std::string tutorialbutton_string;
		static int tutorialbutton_size;
		static sf::Text tutorialbutton;
		static std::string tutoriallevel;
		static sf::Font font;
		static bool pressed;
		static int view_state;
		static EventDelegate delegate;
	public:
		static void Create(const char* resource);
		static void update(sf::RenderWindow *window, int* state, float time);
		static void update(EventInterfacePtr e);
		static void render(sf::RenderWindow *window);

};

#endif
