#include "Configuration.h"
Configuration* Configuration::globalConfigurationPtr = nullptr;

/** Destructor
 ** 
**/
Configuration::~Configuration() {
  // free the memory
  delete globalConfigurationPtr;
}

/** Returns a pointer to the configuration
 **
**/
Configuration* Configuration::instance() {
  if (!globalConfigurationPtr) {
    globalConfigurationPtr = new Configuration();
  }
  return globalConfigurationPtr;
}

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
unsigned Configuration::getWindowWidth()  const {
  return window_width;
}

/** Returns the height of the game window
 **
**/
unsigned Configuration::getWindowHeight() const {
  return window_height;
}
