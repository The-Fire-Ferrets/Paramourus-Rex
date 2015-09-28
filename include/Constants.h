#include <SFML/Graphics.hpp>
#include "pugixml.hpp"
#include <iostream>

#include <map>
#include <memory>
#include <tr1/memory>

#include <math.h>
#include <string.h>
#include <string>

#define PI 3.14159265

class Actor;
class ActorComponent;

typedef std::string ActorId;
typedef std::string ComponentId;

typedef std::shared_ptr<Actor> StrongActorPtr;
typedef std::shared_ptr<ActorComponent> StrongActorComponentPtr;
typedef ActorComponent *(*ActorComponentCreator)(void);
typedef std::map<ComponentId, ActorComponentCreator> ActorComponentCreatorMap;
typedef std::map<ComponentId, StrongActorComponentPtr> ActorComponents;
