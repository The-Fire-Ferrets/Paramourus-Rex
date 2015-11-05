#ifndef TITLEVIEW_H
#define TITLEVIEW_H

#include "Constants.h"
#include "MapView.h"

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
		static std::string tutorialbutton_string;
		static int tutorialbutton_size;
		static sf::Text tutorialbutton;
		static sf::Font font;
		static bool playbutton_pressed;
	public:
		static void Create(const char* resource);
		static void update(sf::RenderWindow *window, int* state, float time);
		static void update(EventInterfacePtr e);
		static void render(sf::RenderWindow *window);

};

#endif
