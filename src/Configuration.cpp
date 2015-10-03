#include "Configuration.h"

/** Destructor
 ** 
**/
Configuration::~Configuration()
{
  delete globalConfigurationPtr;
}

/** Returns a pointer to the configuration
 **
**/
ConfigurationPtr Configuration::get() const
{
  return globalConfigurationPtr;
}

/** Sets pointer to the game configuration
 **
**/
void Configuration::set(ConfigurationPtr const ptr)
{
  globalConfigurationPtr = ptr;
}
