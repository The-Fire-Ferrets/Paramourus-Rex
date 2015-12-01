#ifndef DIALOGUEVIEW_H
#define DIALOGUEVIEW_H

#include "Constants.h"
#include "ActorFactory.h"
#include <vector>
#include <utility>
#include "LevelView.h"
#include "CraftView.h"
#include "TitleView.h"
#include "MapView.h"

class DialogueView {
	friend class LevelView;
	friend class TitleView;
	friend class CraftView;
	friend class MapView;
	protected:
		static int view_state;
	private:
		static std::string name;
		static int numDialogues;
		static const int size;
		static int index;
		static std::string dialogue_str;
		
		//images
		static sf::Texture background_texture;
		static sf::Sprite background;
		//static sf::RectangleShape backlay;
		static sf::Texture backlay_texture;
		static sf::Sprite backlay;

		// text values
		static sf::Font font;
		static sf::Text text;

		// character art
		static sf::Texture  lhs_character_tex;
		static sf::Sprite   lhs_character_sprite;

		static sf::Texture  rhs_character_tex;
		static sf::Sprite	rhs_character_sprite;
		static sf::Vector2f dialogue_pos;

		static bool pressed;

		static std::vector<std::string> fitStringToBox(std::string str, int character_size = 0, sf::Vector2f box_size = sf::Vector2f(0, 0), bool center = true);

		static sf::SoundBuffer buffer;
		static sf::Sound sound;

		static bool player_response;
		static bool solved;
		static bool draw_overlay;
		static int response;

		// commentary display
		static sf::Texture commentary_prompt_texture;
		static sf::Sprite  commentary_prompt;
		static std::string commentary_string_flower;
		static std::string commentary_string_choices;
		static sf::Text    commentary_flower;
		static sf::Text    commentary_choices;
		static bool        display_commentary_flower;
		static bool        display_commentary_choices;

	public:
		static int num_times_impressed;
		static std::vector<std::pair<std::string, std::string>> boxes;     // what gets displayed on screen
		static std::vector<std::pair<std::string, std::string>> dialogues; // what gets read in from the XML
		static void Create(const char* resource, int* state);
		static void update(sf::RenderWindow *window, int* state);
		static void update(EventInterfacePtr e);
		static void render(sf::RenderWindow *window);
		static void cleanUp();
};

#endif
