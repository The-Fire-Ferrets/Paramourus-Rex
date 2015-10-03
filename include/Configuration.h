#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include "Constants.h"

class Configuration
{
  private:
    static ConfigurationPtr globalConfigurationPtr;

  public:
    explicit Configuration() {}
    ~Configuration();

    static ConfigurationPtr get() const;
    static void             set(ConfigurationPtr const ptr);

    // game configuration parameters
    unsigned window_height, window_width;
}
#endif
