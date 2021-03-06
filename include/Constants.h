#include <SFML/Graphics.hpp>
#include <SFML/Audio/SoundBuffer.hpp>
#include <SFML/Audio/Sound.hpp>
#include "pugixml.hpp"
#include <iostream>

#include <map>
#include <thread>
#include <memory>
#include <tr1/memory>
#include <unordered_set>
#include <cassert> //  assert()
#include <algorithm> // std::find()
#include <mutex>
#include <math.h>
#include <string.h>
#include <string>
#include <list>
#include <vector>
#include <cfloat>         // for FLT_EPSILON
#include <iomanip>
#include <sstream>
#include "FastDelegate.h"
#include "Configuration.h"
#include "Pathfinder.h"
#include "Utilities.h"

#define PI 3.14159265

class Actor;
class ActorComponent;
class EventInterface;
class EventManagerInterface;
class ContactEvent;
class CollectEvent;
class Configuration;

typedef std::string ActorId;
typedef unsigned long ActorInstance;
typedef std::string ComponentId;

typedef std::shared_ptr<Actor> StrongActorPtr;
typedef std::shared_ptr<ActorComponent> StrongActorComponentPtr;
typedef ActorComponent *(*ActorComponentCreator)(void);
typedef std::map<ComponentId, ActorComponentCreator> ActorComponentCreatorMap;
typedef std::map<ComponentId, StrongActorComponentPtr> ActorComponents;

typedef unsigned long EventType;
typedef std::shared_ptr<EventManagerInterface> EventManagerInterfacePtr;
typedef std::shared_ptr<const EventInterface> EventInterfacePtr;
typedef fastdelegate::FastDelegate1<EventInterfacePtr> EventDelegate;
typedef std::list<const EventInterface*> EventQueue;
typedef std::shared_ptr<EventQueue> EventQueuePtr;
typedef std::list<EventDelegate> EventDelegateList;
typedef std::pair<const EventType, const ActorInstance> EventInstance;
typedef std::map<const EventInstance, EventDelegateList> EventDelegateMap;
