#include "Actor.h"
#include "Constants.h" // for window_[width|height]
#include "PhysicsComponent.h"
//unique instance id among actors
int Actor::instances = 0;
//Number of directions possible
const int Actor::num_directions = 4;
/** Gets the current actors ID
 **
 **/
ActorId Actor::getId(void) {
    return id;
}

/** Constructor
 ** actorid : unique id for each actor
 ** Sets some attributes to their defaults
 **/
Actor::Actor(void) {
	instance = instances++;
	state = 0;
	visible = true;
}


/** Destructor
 **
 **/
Actor::~Actor(void) {

}

/** Initializer
 ** elem : node pointing to section of XML configuration holding more attribute defaults to setup
 ** Sets up additional attribute defaults
 **/
bool Actor::Init(pugi::xml_node* elem) {
    for (pugi::xml_attribute attr = elem->first_attribute(); attr; attr = attr.next_attribute()) {
        if (!strcmp(attr.name(),"Type"))
            id = attr.value();
        else if (!strcmp(attr.name(),"SpriteUp") || !strcmp(attr.name(),"Sprite")) {
            sprite_filename[0] = attr.value();
	}
	else if (!strcmp(attr.name(),"SpriteDown"))
            sprite_filename[1] = attr.value();
	else if (!strcmp(attr.name(),"SpriteLeft"))
            sprite_filename[2] = attr.value();
	else if (!strcmp(attr.name(),"SpriteRight"))
            sprite_filename[3] = attr.value();
        else {
            std::cout << "Actor::Init: Failed to initialize" << std::endl;
            return false;
        }
    }
    boundary = new sf::FloatRect();
    addDelegate(ContactEvent::event_type);
    return true;
}

/** Final Initializer
 ** Setups up additional attributes based on game configuration
 **/
void Actor::PostInit(pugi::xml_node* elem) {
    char* temp;
    for (pugi::xml_node tool = elem->first_child(); tool; tool = tool.next_sibling()) {
        for (pugi::xml_attribute attr = tool.first_attribute(); attr; attr = attr.next_attribute()) {
            if (!strcmp(attr.name(),"X")) {
                position.x = std::strtol(attr.value(), &temp, 10);
                if (*temp != '\0') {
                    std::cout << "Actor::PostInit: Failed to post-initialize: Error reading attribute for " << attr.name() << std::endl;
                }
            }
            else if (!strcmp(attr.name(),"Y")) {
                position.y = std::strtol(attr.value(), &temp, 10);
                if (*temp != '\0') {
                    std::cout << "Actor::PostInit: Failed to post-initialize: Error reading attribute for " << attr.name() << std::endl;
                }
            }
            else if (!strcmp(attr.name(),"Width")) {
                size.x = std::strtol(attr.value(), &temp, 10);
                if (*temp != '\0') {
                    std::cout << "Actor::PostInit: Failed to post-initialize: Error reading attribute for " << attr.name() << std::endl;
                }
            }
            else if (!strcmp(attr.name(),"Height")) {
                size.y = std::strtol(attr.value(), &temp, 10);
                if (*temp != '\0') {
                    std::cout << "Actor::PostInit: Failed to post-initialize: Error reading attribute for " << attr.name() << std::endl;
                }
            }
        }
    }

    sf::Vector2f pos = position;
    if (pos.x < 0 || pos.y < 0) {
        bool new_position = false;
        bool conflict = false;
        while (!new_position) {
            pos.x = rand() % Configuration::getWindowWidth();
            pos.y = rand() % Configuration::getWindowHeight();

            std::vector<StrongActorPtr>::iterator it_all;
            for (it_all = LevelView::actorList.begin(); it_all != LevelView::actorList.end(); it_all++) {
                if ((*it_all)->getPosition() == pos) {
                    conflict = true;
                    break;
                }
            }
            if (!conflict)
                new_position = true;
            conflict = false;
        }
    }
    position = pos;
    boundary = new sf::FloatRect(position.x, position.y, size.x, size.y);
	for (int i = 0; i < num_directions; i++) {
		if (!sprite_filename[i].empty())
    			sprite_texture[i].loadFromFile(("./assets/sprites/" + sprite_filename[i]).c_str());
		else
			sprite_texture[i].loadFromFile(("./assets/sprites/" + sprite_filename[0]).c_str());
    		sprite[i] = sf::Sprite(sprite_texture[i], sf::IntRect(0, 0, (sprite_texture[i].getSize()).x, (sprite_texture[i].getSize()).y));
   		sprite[i].setScale(size.x/(sprite_texture[i].getSize()).x, size.y/(sprite_texture[i].getSize()).y);
    		sprite[i].setPosition(position);
	}
	sprite_idx = 0;
}


/** Moves the actor a certain distance based on the current game time
 ** time: current game time
 ** Prevents actors from moving past the top and bottom boundaries
 ** if the obstacle pointer is set (by a physics component o fthe actor or another actor after contact) it prevents the actors from moving into each other
 **/
    void Actor::move(float distance, sf::Vector2f direction) {
        //Move Actor
        sf::Vector2f p = this->getPosition() + direction * distance;

        // disallow movement off the screen
        unsigned width = Configuration::getWindowWidth();
        unsigned height = Configuration::getWindowHeight();

	if (direction.x < 0)
		sprite_idx = 2;
	else if (direction.x > 0)
		sprite_idx = 3;
	else if (direction.y < 0)
		sprite_idx = 0;
	else if (direction.y > 0)
		sprite_idx = 1;

        if (p.x < FLT_EPSILON)
            p.x = FLT_EPSILON;
        if (p.y < FLT_EPSILON)
            p.y = FLT_EPSILON;
        if (p.x > width-size.x)
            p.x = width - size.x;
        if (p.y > height-size.y)
            p.y = height - size.y;

        //Get the bounds after movement and check if the movement is allowed
        sf::FloatRect bound_after = sf::FloatRect(p + direction * distance, getSize());
        std::shared_ptr<ActorComponent>     ac;
        std::shared_ptr<PhysicsComponent>   pc;
        ac = components[PhysicsComponent::id];
        pc = std::dynamic_pointer_cast<PhysicsComponent>(ac);
        if (pc->query(bound_after, direction)) {
            // set the position
            this->setPosition(p + direction * distance);
        }
    }

/** Updates each of the actor's components
 ** time: current game time
 **/
void Actor::update(float time) {
    for (ActorComponents::iterator it = components.begin(); it != components.end(); ++it)
        (it->second)->update(time);
}

/** Renders each of the actors components
 ** window: current game render window
 **/
        void Actor::render(sf::RenderWindow *window) {
            if (visible)
                window->draw(sprite[sprite_idx]);
        }

/** Reset each of the actors components after scoring
 **
 **/
                    void Actor::reset(void) {
                        for (ActorComponents::iterator it = components.begin(); it != components.end(); ++it)
                            (it->second)->reset();
                    }

/** Restarts each of the actors components after someone wins
 **
 **/
void Actor::restart(void) {
    for (ActorComponents::iterator it = components.begin(); it != components.end(); ++it)
        (it->second)->restart();
}

/** Cleans up each of the actors components after quiting
 **
 **/
void Actor::quit(void) {
    for (ActorComponents::iterator it = components.begin(); it != components.end(); ++it)
        (it->second)->quit();

    for (std::vector<EventDelegate>::iterator it = delegateFuncList.begin(); it != delegateFuncList.end(); it++)
        if(!EventManagerInterface::get()->removeDelegate(*it))
            std::cout << "Actor::~Actor: Unable to unregister delegate function" << std::endl;

}

/** Adds a component to the actor
 ** component: pointer to ActorComponent ot be added
 **/
        void Actor::AddComponent(StrongActorComponentPtr component) {
            components.insert({component->getId(), component});
        }

/** Updates the actors boundaries to be used by a PhysicsComponent
 **
 **/
    void Actor::updateBoundary(void) {
        *boundary = sf::FloatRect(position.x, position.y, size.x, size.y);
    }


// Mutators and accesors

/** returns the actors boundary
 **
 **/
        sf::FloatRect* Actor::getBoundary(void) {
            return boundary;
        }

/** returns the actors state
 **
 **/
int Actor::getState(void) {
    return state;
}

/** sets the actors state
 ** actorState: new state to have actor in
 **/
void Actor::setState(int actorState) {
    state = actorState;
}

/** Returns the last game state the actor was aware of
 **
 **/
int* Actor::getGameState(void) {
    return game_state;
}

/** Makes the actor aware of the current game state
 **
 **/
                            void Actor::setGameState(int state) {
                                *game_state = state;
                            }

/** Changes the game state
 **
 **/
    void Actor::resetGameState(int* state) {
        game_state = state;
    }

/** Sets the height and width dimensions of actor
 ** sz: holds height and width data
 **/
void Actor::setSize(sf::Vector2f sz) {
    size = sz;
    updateBoundary();
}

/** Returns the actors size
 **
 **/
sf::Vector2f Actor::getSize() {
    return size;
}

/** Reutrns the actor's current position
 **
 **/
sf::Vector2f Actor::getPosition(void) {
    return position;
}

/** Sets the actors to the new position
 **
 **/
void Actor::setPosition(sf::Vector2f pos) {
    position = pos;
    updateBoundary();
	for (int i = 0; i < num_directions; i++)
    		sprite[i].setPosition(position);
}

/** Return the actors instance
 **
 **/
            int Actor::getInstance(void) {
                return instance;
            }

/** Return a reference to the current actor
 **
 **/
    Actor* Actor::getCopy(void) {
        return this;
    }

const Actor* Actor::getCopy(void) const {
    return this;
}

/** Receives events when the actor contacts another actor
 ** e: event received
 ** Sends each of its components to process the event according to their implementations
 **/
void Actor::getEvent(EventInterfacePtr e) {
    EventType event_type = e->getEventType();
    if (event_type == ContactEvent::event_type)
        std::cout << LevelView::getActor(e->getSender())->getId() << " made contact with " << id << std::endl;

    for (ActorComponents::iterator it = components.begin(); it != components.end(); ++it)
        (it->second)->update(e);
}

/** Used to check of the actor contains a given compoment
 ** Component: component id to check if contained
 **/
bool Actor::hasComponent(ComponentId component) {
    for (ActorComponents::iterator it = components.begin(); it != components.end(); ++it)
        if ((it->second)->getId() == component)
            return true;
    return false;
}

/** Sets whether the actor is rendered or not
 ** v: visibility to set
 **/
void Actor::setVisible(bool v) {
    visible = v;
}

/** Returns whether or not the actor is rendering itself
 ** 
 **/
bool Actor::getVisible(void) {
    return visible;
}

/** Adds listener for given event to this actor
 **
 **/
void Actor::addDelegate(EventType type) {
    delegateFuncList.push_back(fastdelegate::MakeDelegate(getCopy(), &Actor::getEvent));
    if(!EventManagerInterface::get()->addDelegate(delegateFuncList.back(), EventInstance(type, getInstance()))) {
        std::cout << "Actor::Init: Unable to register delegate function" << std::endl;
    }
}

/** Checks to see if the Actor has the given attribute by checking the type of its components
 **
 **/
    bool Actor::hasAttribute(std::string attr) {
        for (ActorComponents::iterator it = components.begin(); it != components.end(); ++it)
            if ((it->second)->getType() == attr)
                return true;
        return false;
    }
