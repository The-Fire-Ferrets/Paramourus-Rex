#ifndef PARAMOURUSREX_H
#define PARAMOURUSREX_H

#include "Constants.h"
#include "LevelView.h"
#include "MapView.h"
#include "EventManager.h"
#include "DialogueView.h"

//Timing and tracking
sf::Clock fr_clock;
sf::Time elapsed;
float elapsed_ms;

//To Load configuration
std::string config = "Config";
pugi::xml_document doc;
pugi::xml_parse_result result;

#endif
