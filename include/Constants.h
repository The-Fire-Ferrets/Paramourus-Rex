#include <SFML/Graphics.hpp>
#include "ActorFactory.h"
#include "LevelFactory.h"
#include <map>
#include <memory>
#include <tr1/memory>
#include <math.h>
#include <string.h>
#include <string>
#include <iostream>
#include <vector>
#include "pugixml.hpp"
#include <cassert>
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
<<<<<<< HEAD
=======

const bool debug_mode = false;
#endif
>>>>>>> e578df3d70403c6a9a0e598ff7212667d14c8476
