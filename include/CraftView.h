#ifndef CRAFTVIEW_H
#define CRAFTVIEW_H

#include "Constants.h"
#include "ActorFactory.h"
#include "CollectorComponent.h"
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
        static std::string name;
        static sf::Clock level_clock;
        static sf::Text text;
        static sf::Font font;
        static std::string flower_list[];
        static sf::Vector2f text_pos;
        static void cleanUp(void);
    public:
        static int getNumFlowers(void);
        static void start();
        static void Create(const char *resource, int *state);
        static void update(EventInterfacePtr e);
        static void update(sf::RenderWindow *window, int *state);
        static void quit(void);
        static void render(sf::RenderWindow *window);
};

#endif



