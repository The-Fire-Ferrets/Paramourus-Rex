#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include <SFML/Graphics.hpp>
/** A static class to hold configuration variables. */
class Configuration
{
    private:
        static unsigned int window_height;
        static unsigned int window_width;
	static unsigned int minimapview_height;
	static unsigned int minimapview_width;
	static unsigned int gameview_height;
	static unsigned int gameview_width;
	static sf::Vector2f gameview_center;

    public:
        static void setWindowDimensions(unsigned width, unsigned height);
	static void setGameViewDimensions(unsigned width, unsigned height);
	static void setGameViewCenter(sf::Vector2f center);
	static void setMiniMapViewDimensions(unsigned width, unsigned height);
        static unsigned int getWindowWidth();
        static unsigned int getWindowHeight();

	static unsigned int getGameViewWidth();
	static unsigned int getGameViewHeight();
	static sf::Vector2f getGameViewCenter();
	
	static unsigned int getMiniMapViewWidth();
	static unsigned int getMiniMapViewHeight();
};
#endif
