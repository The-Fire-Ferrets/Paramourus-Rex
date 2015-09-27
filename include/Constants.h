#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <SFML/Graphics.hpp>
#include <map>
#include <memory>
#include <tr1/memory>
#include <math.h>
#include <string.h>
#include <iostream>
#include <vector>
#include "pugixml.hpp"
#include <cassert>

#define PI 3.14159265

class Actor;
class ActorComponent;

typedef unsigned long ActorId;
typedef std::string ComponentId;

typedef std::shared_ptr<Actor> StrongActorPtr;
typedef std::shared_ptr<ActorComponent> StrongActorComponentPtr;
typedef ActorComponent *(*ActorComponentCreator)(void);
typedef std::map<ComponentId, ActorComponentCreator> ActorComponentCreatorMap;
typedef std::map<ComponentId, StrongActorComponentPtr> ActorComponents;

const bool debug_mode = False;
#endif
