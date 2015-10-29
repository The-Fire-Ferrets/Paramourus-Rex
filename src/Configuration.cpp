#include "Configuration.h"

//WIndow Height
unsigned int Configuration::window_height = 0;
unsigned int Configuration::window_width = 0;

unsigned int Configuration::minimapview_height = 0;
unsigned int Configuration::minimapview_width = 0;
sf::Texture Configuration::minimap_border;

unsigned int Configuration::gameview_height = 0;
unsigned int Configuration::gameview_width = 0;
sf::Vector2f Configuration::gameview_center;

sf::Texture Configuration::loading_texture;
sf::Sprite Configuration::loading_sprite;

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

/** Returns minimap border
 **
 **/
sf::Texture Configuration::getMinimapBorder(void) {
	return minimap_border;
}

/** Set minimapborder
 **
 **/
void Configuration::setMinimapBorder(sf::Texture mb) {
	minimap_border = minimap_border;
}

/** Get loading texture
 **
 **/
sf::Texture Configuration::getLoadingTexture(void) {
	return loading_texture;
}

/** Set loading texture
 **
 **/
void Configuration::setLoadingTexture(sf::Texture t) {
	loading_texture = t;
	loading_sprite = sf::Sprite(loading_texture, sf::IntRect(0, 0, loading_texture.getSize().x, loading_texture.getSize().y));
	loading_sprite.setPosition(sf::Vector2f(window_width/2 - loading_texture.getSize().x/2, window_height/2 - loading_texture.getSize().y/2));
}

/** Get loading sprite
 **
 **/
sf::Sprite Configuration::getLoadingSprite(void) {
	return loading_sprite;
}
