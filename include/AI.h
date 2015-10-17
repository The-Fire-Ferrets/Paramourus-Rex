#ifndef AI_H
#define AI_H

#include <SFML/System/Vector2.hpp>
#include <vector>
#include "Utilities.h"

class AI {
    public:
        AI() {};
        AI(sf::Vector2f pos);

        void setNPCPosition(const sf::Vector2f& pos);
        sf::Vector2f getNPCPosition(void) const;

        sf::Vector2f findClosestFlower(void) const;
        sf::Vector2f findPlayer(void) const;
        sf::Vector2f chooseDirection(const sf::Vector2f dest) const;
        bool hasCollisionOnPath(const sf::Vector2f& dir) const;

    private:
        sf::Vector2f npc_pos;

		sf::Vector2f selectNextDirection(const sf::Vector2f& distance, const std::vector<sf::Vector2f>& excluded) const;
};

#endif/*AI_H*/
