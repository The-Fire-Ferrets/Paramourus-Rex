#include "Configuration.h"

//WIndow Height
unsigned int Configuration::window_height = 0;
unsigned int Configuration::window_width = 0;

unsigned int Configuration::minimapview_height = 0;
unsigned int Configuration::minimapview_width = 0;

unsigned int Configuration::gameview_height = 0;
unsigned int Configuration::gameview_width = 0;
sf::Vector2f Configuration::gameview_center;

/** Sets the dimensions of the game window
 **
 **/
void Configuration::setWindowDimensions(unsigned width, unsigned height) {
    window_width = width;
    window_height = height;
}

/** Sets the dimensions of the game view
 **
 **/
void Configuration::setGameViewDimensions(unsigned width, unsigned height) {
    gameview_width = width;
    gameview_height = height;	
}

/** Sets the dimensions of the game view
 **
 **/
void Configuration::setGameViewCenter(sf::Vector2f center) {
    gameview_center = center;	
}

/** Sets the dimensions of the minimap
 **
 **/
void Configuration::setMiniMapViewDimensions(unsigned width, unsigned height) {
    minimapview_width = width;
    minimapview_height = height;
}

/** Returns the width of the game window
 **
 **/
unsigned int Configuration::getWindowWidth() {
    return window_width;
}

/** Returns the height of the game window
 **
 **/
unsigned int Configuration::getWindowHeight() {
    return window_height;
}

/** Returns the width of the game window
 **
 **/
unsigned int Configuration::getGameViewWidth() {
    return gameview_width;
}

/** Returns the height of the game window
 **
 **/
unsigned int Configuration::getGameViewHeight() {
    return gameview_height;
}

/** Returns the center of the game window
 **
 **/
sf::Vector2f Configuration::getGameViewCenter() {
    return gameview_center;
}

/** Returns the width of the game window
 **
 **/
unsigned int Configuration::getMiniMapViewWidth() {
    return minimapview_width;
}

/** Returns the height of the game window
 **
 **/
unsigned int Configuration::getMiniMapViewHeight() {
    return minimapview_height;
}
