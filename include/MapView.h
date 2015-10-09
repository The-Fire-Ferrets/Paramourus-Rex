#ifndef MAPVIEW_H
#define MAPVIEW_H

#include "Constants.h"
#include "LevelView.h"
class MapView {
	private:
		static const int size;
		static int num_levels;
		static sf::Sprite background;
		static sf::Texture background_texture;
		static std::string levels[];
		static sf::Sprite sprites[];
		static sf::Texture textures[];
		static sf::Vector2f positions[];
		static sf::Vector2f sizes[];
	public:
		static void Create(const char* resource);
		
		static void update(sf::RenderWindow *window, int* state, float time);
		static void update(EventInterfacePtr e);
		static void render(sf::RenderWindow *window);
	
};

#endif
