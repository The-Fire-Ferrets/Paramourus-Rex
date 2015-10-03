#ifndef CONFIGURATION_H
#define CONFIGURATION_H

/** A singleton class to hold configuration variables. */
class Configuration
{
  private:
    // TODO: I want this to be a std::unique_ptr, but I was having issues 
    // getting the project to build.
    static Configuration* globalConfigurationPtr;
    Configuration() {};

    // game configuration parameters
    unsigned window_height, window_width;

  public:
    ~Configuration();

    static Configuration* instance();

    // setters and getters for configuration
    void setWindowDimensions(unsigned width, unsigned height);
    unsigned getWindowWidth()  const;
    unsigned getWindowHeight() const;
};
#endif
