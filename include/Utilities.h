#ifndef UTILITIES_H
#define UTILITIES_H

#include <SFML/System/Vector2.hpp>

float distance(sf::Vector2f p, sf::Vector2f q);
bool slope(sf::Vector2f p, sf::Vector2f q, float& m);

#endif/*UTILITIES_H*/
