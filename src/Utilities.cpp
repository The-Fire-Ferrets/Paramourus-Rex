#include "Utilities.h"
#include <cmath> // std::pow(), std::sqrt(), std::abs()
#include <cfloat> // FLT_EPSILON
#include <sstream>
#include <string>
#include <algorithm> // std::equal(), std::sort()
#include <iostream>

/** Returns the distance between the points p and q
 **/
float distance(sf::Vector2f p, sf::Vector2f q) {
    return std::sqrt( std::pow(p.x-q.x, 2) + std::pow(p.y-q.y, 2) );
}

/** Returns  the slope of the line segment defined by the points p and q
 **/
bool slope(sf::Vector2f p, sf::Vector2f q, float& m) {
    float dx = p.x - q.x;
    float dy = p.y - q.y;

    if (std::abs(dx) <= FLT_EPSILON) {
        return false;
    }
    m = dy/dx;
    return true;
}

/** Splits the input string at the spaces.
 **/
std::vector<std::string> split(std::string str, char delim) {
	std::stringstream ss(str);
	std::vector<std::string> words;
	std::string word;

	while (std::getline(ss, word, delim)) {
		words.push_back(word);
	}

	return words;
}

/** Checks for component equivalence in two string vectors.
 **
 **/
bool have_equivalent_strings(std::vector<std::string> lhs, std::vector<std::string> rhs) {
	// trivial check
	if (lhs.size() != rhs.size()) {
		return false;
	}

	// sort the input vectors
	std::sort(lhs.begin(), lhs.end());
	std::sort(rhs.begin(), rhs.end());

	// return equality of sorted vectors
	return std::equal(lhs.begin(), lhs.end(), rhs.begin());
}
