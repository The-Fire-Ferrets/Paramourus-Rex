#ifndef CONFIGURATION_H
#define CONFIGURATION_H

/** A static class to hold configuration variables. */
class Configuration
{
	private:
		static unsigned int window_height;
		static unsigned int window_width;

	public:
		static void setWindowDimensions(unsigned width, unsigned height);
		static unsigned int getWindowWidth();
		static unsigned int getWindowHeight();
};
#endif
