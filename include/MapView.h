#ifndef MAPVIEW_H
#define MAPVIEW_H

#include "Constants.h"
#include "LevelView.h"
#include "DialogueView.h"
#include "CraftView.h"
#include "TitleView.h"
#include "CollectorComponent.h"

typedef std::pair<ActorId, ActorId> ContactPair;
typedef std::pair<ActorId, ContactPair> DisplayContactPair;

class MapView {
	friend class LevelView;
	friend class DialogueView;
	friend class TitleView;
	friend class CraftView;
	protected:
		static int view_state;
		static int commentary_idx;
		static bool reset;
		static int level_idx;
	private:
		static const int size;
		static int num_levels;
		static sf::Sprite background;
		static sf::Texture background_texture;
		static std::string levels[];
		static sf::Sprite sprites[];
		static sf::Texture textures[];
		static sf::Sprite title_sprite;
		static sf::Texture title_texture;
		static sf::Vector2f title_size;		
		static sf::Vector2f positions[];
		static sf::Vector2f sizes[];
		static float fireflowers[];
		static float earthflowers[];
		static float airflowers[];
		static float waterflowers[];
		static int fireflowers_count[];
		static int earthflowers_count[];
		static int airflowers_count[];
		static int waterflowers_count[];
		static std::map<int, sf::Text> commentary;
		static std::map<DisplayContactPair, sf::Vector2f> commentary_positions;
		static std::map<DisplayContactPair, std::vector<std::string>> commentary_strings;
		static std::map<DisplayContactPair, int> commentary_occurance;
		static std::map<DisplayContactPair, int> commentary_actions;
		static sf::Vector2f commentary_pos;
		static std::map<int, sf::Clock> commentary_timer;
		static std::string fitStringToCommentaryBox(std::string str);
		static std::string flowers_string[];
		static sf::Text flowers_text[];
		static sf::Font font;
		static int flowers[];
		static bool pressed;
		static void resetPopulationValues(void);
		static EventDelegate delegate;
		static int min_flowers[];
		static int max_flowers[];
    // quit game confirmation
		static sf::Text back_continue;
		static sf::Text back_cancel;
		static sf::Text back_message;
		static sf::Text back_question;
		static std::string back_continue_text;
		static std::string back_cancel_text;
		static std::string back_message_text;
		static std::string back_question_text;
	public:
		static void Create(const char* resource);
		static void update(sf::RenderWindow *window, int* state, float time);
		static void update(EventInterfacePtr e);
		static void render(sf::RenderWindow *window);

};

#endif
