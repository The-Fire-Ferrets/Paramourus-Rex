#ifndef CRAFTVIEW_H
#define CRAFTVIEW_H

#include "Constants.h"
#include "ActorFactory.h"
#include "CollectorComponent.h"
#include "CraftableComponent.h"
#include "ActorComponent.h"
#include "LevelView.h"

class CraftView  {
	private:
		static const int size;
		static int totalFlowers;
		static int fireFlowers;
		static int waterFlowers;
		static int earthFlowers;
		static int airFlowers;
		static sf::Texture background_texture;
		static sf::Sprite background;
		static sf::Sprite map;
		static sf::Sprite book;
		static sf::Sprite box1Sprite;
		static sf::Sprite box2Sprite;
		static sf::Sprite sprites[];
		static sf::Vector2f sizes[];
		static sf::Vector2f positions[];
		static sf::Texture textures[];
		static std::string name;
		static sf::Clock level_clock;
		static sf::Text text;
		static sf::Font font;
		static std::string flower_list[];
		static sf::Vector2f text_pos;
		static void cleanUp(void);
		static std::string flower_str;
		static sf::Text flower_text;
		static bool box1;
		static bool box2;
		static sf::RectangleShape backlay;
		static sf::RectangleShape craftButton;
		static sf::Text button_text;

		static std::vector<EventDelegate> delegateFuncList;
		static bool has_delegates;

	public:
		static int getNumFlowers(void);
		static bool pressed;
		static void start();
		static void Create(const char *resource, int *state);
		static void update(EventInterfacePtr e);
		static void update(sf::RenderWindow *window, int *state);
		static void quit(void);
		static void render(sf::RenderWindow *window);
		static std::vector<StrongActorPtr> actorList;
		static StrongActorPtr getFlower(int instance);
		static bool removeFlower(StrongActorPtr flower);
		static void addDelegate(EventType type);


};

#endif



