#ifndef CRAFTVIEW_H
#define CRAFTVIEW_H

#include "Constants.h"
#include "ActorFactory.h"
#include "CollectorComponent.h"
#include "CraftableComponent.h"
#include "ActorComponent.h"
#include "LevelView.h"
#include "DialogueView.h"
#include "TitleView.h"
#include "MapView.h"

class CraftView  {
	friend class LevelView;
	friend class DialogueView;
	friend class TitleView;
	friend class MapView;
	protected:
		static int view_state;
    private:	
	// Flower counts
        static int totalFlowers;
        static int fireFlowers;
        static int waterFlowers;
        static int earthFlowers;
        static int airFlowers;
        static int sunFlowers;
        static int tulips;
        static int roses;
        static int violets;
        static int lilies;
        static int orchids;
        static int magnolias;

	static std::string flower_list[];
	
	// Sprites
	static sf::Sprite sprites[];
	static sf::Sprite background;
	static sf::Sprite map;
	static sf::Sprite recipe_scroll_sprite;
	static sf::Sprite scroll_icon_sprite;
	static sf::Sprite diana_icon_sprite;
	static sf::Sprite map_icon_sprite;
	// static sf::Sprite hints_icon_sprite;

	static sf::Sprite box1Sprite;
	static sf::Sprite box2Sprite;
	
	// Text
	static sf::Text text;
	static sf::Font font;
	static std::string flower_str;
	static sf::Text flower_text;
	
	//static sf::RectangleShape hints_overlay;
	static sf::Texture hints_prompt_texture;
	static sf::Sprite hints_prompt;
	static sf::Text hints_text;
	static int hints_size;

	// Position variables
	static sf::Vector2f sizes[];
	static sf::Vector2f positions[];
	static sf::Vector2f text_pos;
	static sf::Vector2f hints_text_pos;

	// Textures
        static sf::Texture background_texture;
	static sf::Texture textures[];
        //static sf::RectangleShape backlay;

	static sf::RectangleShape craft_button;
	static sf::Texture scroll_texture;
	static sf::Texture map_texture;
	static sf::Texture diana_icon_texture;
	static sf::Texture scroll_icon_texture;
	// static sf::Texture hints_icon_texture;

	// backlay
	static sf::Texture backlay_texture;
	static sf::Sprite backlay;

	// exit buttons
	static sf::RectangleShape map_button, dialogue_button;
	static bool has_crafted;
	static bool first_click;
	
	// Determining occupation of craftTable
	static bool box1;
	static bool box2;
	
	// Determining if recipe book is open or not
	static bool drawBook;
	static bool drawHints;
	
	// Misc
	static void cleanUp(void);
	static const int size;

		static std::vector<EventDelegate> delegateFuncList;
		static bool has_delegates;
	static EventDelegate delegate;
	
	static sf::SoundBuffer buffer;
	static sf::Sound sound;

	static sf::Texture character_tex;
	static sf::Sprite  character_sprite;

	static std::string fitStringToDialogueBox(std::string str);
	static std::string fitStringToHintsBox(std::string str, int character_size = 0, sf::Vector2f box_size = sf::Vector2f(0,0), bool center = true);

	public:
        static int total_craft_visits;
		static int getNumFlowers(void);
		static bool pressed;
		static void start();
		static void Create(const char *resource);
		static void update(EventInterfacePtr e);
		static void update(sf::RenderWindow *window, int *state);
		static void quit(void);
		static void render(sf::RenderWindow *window);
		static std::vector<StrongActorPtr> actorList;
		static StrongActorPtr selectedActor1;
		static StrongActorPtr selectedActor2;
		static StrongActorPtr getFlower(int instance);
		static StrongActorPtr getFlower(std::string type);
		static bool removeFlower(StrongActorPtr flower);
		static void addDelegate(EventType type);
		static void returnFlower(StrongActorPtr flower);
		static void updateFlowerCount(std::string flower);
    static void clearInventory(void);

		static std::vector<std::tuple<sf::Sprite, std::string>> flowerStrList;



};

#endif



