#ifndef AI_H
#define AI_H

#include <SFML/System/Vector2.hpp>
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
        bool isObstacleInPath(const sf::Vector2f& dest) const;

    private:
        sf::Vector2f npc_pos;
};

#endif/*AI_H*/
