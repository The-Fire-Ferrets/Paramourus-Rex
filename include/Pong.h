#ifndef PONG_H
#define PONG_H

#include "Constants.h"

sf::RenderWindow App(sf::VideoMode(window_width, window_height,64), "Pong by Bina Kakusa", sf::Style::Titlebar|sf::Style::Close);
sf::Clock fr_clock;
sf::Time elapsed;

float elapsed_f;
float lag_f;

#endif
