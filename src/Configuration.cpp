#include "Configuration.h"

//WIndow Height
unsigned int Configuration::window_height = 0;
unsigned int Configuration::window_width = 0;

/** Sets the dimensions of the game window
 **
**/
void Configuration::setWindowDimensions(unsigned width, unsigned height) {
	window_width = width;
	window_height = height;
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
