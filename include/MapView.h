#ifndef MAPVIEW_H
#define MAPVIEW_H

#include "Constants.h"
#include "LevelView.h"
#include "DialogueView.h"
#include "CraftView.h"
#include "CollectorComponent.h"

class MapView {
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
		static std::string flowers_string[];
		static sf::Text flowers_text[];
		static sf::Font font;
		static int flowers;
		static bool pressed;
		static bool reset;
		static void resetPopulationValues(void);
		static int view_state;
		static EventDelegate delegate;
		static int min_flowers;
		static int max_flowers;
	public:
		static void Create(const char* resource);

		static void update(sf::RenderWindow *window, int* state, float time);
		static void update(EventInterfacePtr e);
		static void render(sf::RenderWindow *window);

};

#endif
