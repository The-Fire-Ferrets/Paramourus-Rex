#ifndef LEVELVIEW_H
#define LEVELVIEW_H

#include "Constants.h"
#include "ActorFactory.h"
#include "DialogueView.h"
#include "CraftView.h"
#include "TitleView.h"
#include "MapView.h"

typedef std::pair<ActorId, ActorId> ContactPair;
typedef std::pair<ActorId, ContactPair> DisplayContactPair;

class LevelView  {
	friend class Pathfinder;
	friend class DialogueView;
	friend class TitleView;
	friend class CraftView;
	friend class MapView;
	protected:
		static int view_state;
	private:
		static const int size;
		static int num_actors;
		static sf::Texture background_texture;
		static sf::Texture timeout_texture;
		static sf::Sprite timeout_sprite;
		static sf::Texture minimap_background_texture;
		static sf::Texture edge_texture;
		static sf::Sprite background;
		static sf::Sprite minimap_background;
		static sf::Sprite edge;
		static std::string name;
		static sf::Clock level_clock;
		static sf::Text timer;
		static std::string timer_string;
		static sf::Font font;
		static sf::Vector2f timer_position;
		static sf::View gameView;
		static sf::View minimapView;
		static sf::View pauseView;
		static sf::Sprite minimap_border;
		static int duration;
		static sf::SoundBuffer buffer;
		static sf::Sound sound;
		static std::map<int, sf::Text> commentary;
		static std::map<DisplayContactPair, std::vector<sf::Vector2f>> commentary_positions;
		static std::map<DisplayContactPair, std::vector<std::vector<std::string>>> commentary_strings;
		static std::map<DisplayContactPair, std::vector<int>> commentary_occurance;
		static std::map<DisplayContactPair, std::vector<int>> commentary_actions;
		static std::map<DisplayContactPair, std::vector<std::vector<int>>> commentary_sizes;
		static std::map<int, sf::Clock> commentary_timer;
		static sf::Texture commentary_prompt_texture;
		static sf::Sprite commentary_prompt;
		static sf::Vector2f commentary_pos;
		static int commentary_size;
		static bool display_commentary;
		static std::string fitStringToCommentaryBox(std::string str, int character_size = 0, sf::Vector2f box_size = sf::Vector2f(0,0), bool center = true);
		static sf::Text title;
		static std::string title_text;
		static sf::Text load_state;
		static std::string load_text;
		static EventDelegate delegate;
		static bool commentary_change;
		static std::vector<std::vector<pugi::xml_node>> actions;
		static pugi::xml_document doc;
		static sf::Sprite back_button;
		static sf::Texture title_texture;
		static sf::Vector2f title_size;	
		static bool pressed;
		static int flashing;
		static bool reveal_homer;
		static float timer_time;
		static bool pause_key_pressed;
		static bool back_key_pressed;
		static bool call_key_pressed;
		static bool testing_key_pressed;
		static int* game_state;
		static int last_action;
		static sf::Text back_continue;
		static sf::Text back_cancel;
		static sf::Text back_message;
		static sf::Text back_question;
		static std::string back_continue_text;
		static std::string back_cancel_text;
		static std::string back_message_text;
		static std::string back_question_text;
		static int last_state;
		static bool space_pressed;
		static bool received_new_commentary;
		static bool testing_view;

	public:
		static void generateActor(pugi::xml_node* elem, int* state, int generate = 1);
		static std::vector<StrongActorPtr> actorList;	
		static StrongActorPtr player;
		static StrongActorPtr homer;
		static StrongActorPtr npc;
		static std::string getName(void);
		static int getNumActors(void);
		static void start();
		static void quit(void);
		static StrongActorPtr getActor(int instance);
		static void removeActor(int instance);
		static void cleanUp(void);
		static int flowers_left;
		static int inVision;
		static void Create(const char* resource, int* state, int flowers[]);
		static void update(sf::RenderWindow *window, int* state, float time);
		static void update(EventInterfacePtr e);
		static void render(sf::RenderWindow *window);
		static bool vases_full;
};

#endif
