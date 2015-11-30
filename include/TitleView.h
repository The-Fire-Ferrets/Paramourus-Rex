#ifndef TITLEVIEW_H
#define TITLEVIEW_H

#include "Constants.h"
#include "LevelView.h"
#include "DialogueView.h"
#include "CraftView.h"
#include "MapView.h"

class TitleView {
	friend class LevelView;
	friend class DialogueView;
	friend class CraftView;
	friend class MapView;
	protected:
		static int view_state;
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
		static EventDelegate delegate;
		static bool testing_key_pressed;
		static bool testing_view;
	public:
		static void Create(const char* resource);
		static void update(sf::RenderWindow *window, int* state, float time);
		static void update(EventInterfacePtr e);
		static void render(sf::RenderWindow *window);

};

#endif
