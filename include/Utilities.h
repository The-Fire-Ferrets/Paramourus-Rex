#ifndef UTILITIES_H
#define UTILITIES_H

#include <SFML/System/Vector2.hpp>
#include <vector>
#include <string>

float distance(sf::Vector2f p, sf::Vector2f q);
bool slope(sf::Vector2f p, sf::Vector2f q, float& m);
std::vector<std::string> split(std::string str, char delim);

bool have_equivalent_strings(std::vector<std::string> lhs, std::vector<std::string> rhs);

#endif/*UTILITIES_H*/
