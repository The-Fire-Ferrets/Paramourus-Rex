#include "Actor.h"
#include "Constants.h" // for window_[width|height]
#include "PhysicsComponent.h"
#include "InputComponent.h"
//unique instance id among actors
int Actor::instances = 0;
//Number of directions possible
const int Actor::num_directions = 4;
/** Gets the current actors ID
 **
 **/
ActorId Actor::getId(void) const {
    return id;
}

/** test for equality: two actors have same id
 **
 **/
bool Actor::operator==(const Actor& rhs) const {
	return this->getId() == rhs.getId();
}

/** Constructor
 ** actorid : unique id for each actor
 ** Sets some attributes to their defaults
 **/
Actor::Actor(void) {
	instance = instances++;
	state = 0;
	visible = true;
	renderToGameView = true;
}


/** Destructor
 **
 **/
Actor::~Actor(void) {
	delete obstacle;
	boundary.clear();
	for (auto& kv : components) {
		// decrease the reference count of the pointer
		kv.second.reset();
	}

	this->delegateFuncList.clear();
}

/** Initializer
 ** elem : node pointing to section of XML configuration holding more attribute defaults to setup
 ** Sets up additional attribute defaults
 **/
bool Actor::Init(pugi::xml_node* elem) {
	char* temp;
    for (pugi::xml_attribute attr = elem->first_attribute(); attr; attr = attr.next_attribute()) {
        if (!strcmp(attr.name(),"Type"))
            id = attr.value();
	 else if (!strcmp(attr.name(),"PathType")) {
		path_type  = std::strtol(attr.value(), &temp, 10);
		if (*temp != '\0') {
		    std::cout << "Actor::Init: Failed to post-initialize: Error reading attribute for " << attr.name() << " Value: " << attr.value() << std::endl;
		}
	    }
	else if (!strcmp(attr.name(),"TargetType")) {
		target_type  = std::strtol(attr.value(), &temp, 10);
		if (*temp != '\0') {
		    std::cout << "Actor::Init: Failed to post-initialize: Error reading attribute for " << attr.name() << " Value: " << attr.value() << std::endl;
		}
	    }
        else if (!strcmp(attr.name(),"SpriteUp") || !strcmp(attr.name(),"Sprite")) {
            sprite_filename[0] = attr.value();
		if (sprite_filename[0] == "")
			renderToGameView = false;
	}
	else if (!strcmp(attr.name(),"SpriteDown"))
            sprite_filename[1] = attr.value();
	else if (!strcmp(attr.name(),"SpriteLeft"))
            sprite_filename[2] = attr.value();
	else if (!strcmp(attr.name(),"SpriteRight"))
            sprite_filename[3] = attr.value();
	else if (!strcmp(attr.name(),"SpriteMinimap"))
		if (!strcmp(attr.value(), ""))
			renderToMinimap = false;
		else {
			renderToMinimap = true;
			spriteMinimap_filename = attr.value();
		}
	else if (!strcmp(attr.name(),"Damage")) {
			if(!strcmp(attr.value(),"True"))
               			damage = true;
			else if(!strcmp(attr.value(),"False"))
				damage = false;
	}
	else if (!strcmp(attr.name(),"VertexArray")) {
		if (!strcmp(attr.value(), "")) {
		}
		else {
			use_vertexarray = true;
			int vertices = 0;
			std::string string_temp;
			int v_count = 0;
			int v_idx = 0;
			std::istringstream input;
   			input.str(attr.value());
			while (std::getline(input, string_temp, ' ')) {
				if (v_count == 0) {
					vertices = std::strtol(string_temp.c_str(), &temp, 10);
					if (*temp != '\0') {
					    std::cout << "Actor::PostInit: Failed to post-initialize: Error reading attribute for " << attr.name() << std::endl;
					}
					sprite_vertexarray = sf::VertexArray(sf::TrianglesStrip, vertices);
				}
				else if (v_count % 2) {
					sprite_vertexarray[v_idx].texCoords.x = std::strtol(string_temp.c_str(), &temp, 10);
					sprite_vertexarray[v_idx].position.x = std::strtol(string_temp.c_str(), &temp, 10);
					if (*temp != '\0') {
					    std::cout << "Actor::PostInit: Failed to post-initialize: Error reading attribute for " << attr.name() << std::endl;
					}
				}
				else {
					sprite_vertexarray[v_idx].texCoords.y = std::strtol(string_temp.c_str(), &temp, 10);
					sprite_vertexarray[v_idx++].position.y = std::strtol(string_temp.c_str(), &temp, 10);
					if (*temp != '\0') {
					    std::cout << "Actor::PostInit: Failed to post-initialize: Error reading attribute for " << attr.name() << std::endl;
					}
				}
				v_count++;
			}
			updateBoundary();
		}
	}
			
    }
	if (initial_init) {
	    boundary.push_back(new sf::FloatRect());
	    addDelegate(ContactEvent::event_type);
		initial_init = false;
	}
	/*if (id =="Player") {
		path_type = -4;
	}
	else if (id == "NPC") {
		path_type = -3;
	}
	else if (id == "FireFlower") {
		path_type = -2;
	}
	else if (id == "EarthFlower") {
		path_type = -2;
	}
	else if (id == "WaterFlower") {
		path_type = -2;
	}
	else if (id == "AirFlower") {
		path_type = -2;
	}
	else {
		path_type = -1;
	}*/
	//std::cout << id << " " << path_type << std::endl;
    return true;
}

/** Final Initializer
 ** Setups up additional attributes based on game configuration
 **/
void Actor::PostInit(pugi::xml_node* elem) {
    char* temp;
	Init(elem);
    for (pugi::xml_node tool = elem->first_child(); tool; tool = tool.next_sibling()) {
	if (hasComponent((ComponentId)tool.name())) {
		components[tool.name()]->PostInit(&tool);
	}
	else {
		for (pugi::xml_attribute attr = tool.first_attribute(); attr; attr = attr.next_attribute()) {
			if (!use_vertexarray) {
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
				size.y  = std::strtol(attr.value(), &temp, 10);
				if (*temp != '\0') {
				    std::cout << "Actor::PostInit: Failed to post-initialize: Error reading attribute for " << attr.name() << " Value: " << attr.value() << std::endl;
				}
			    }
			else if (!strcmp(attr.name(),"Damage")) {
				if(!strcmp(attr.value(),"True"))
					damage = true;
				else if(!strcmp(attr.value(),"False"))
					damage = false;
			}
			 else if (!strcmp(attr.name(),"PathType")) {
				path_type  = std::strtol(attr.value(), &temp, 10);
				if (*temp != '\0') {
				    std::cout << "Actor::PostInit: Failed to post-initialize: Error reading attribute for " << attr.name() << " Value: " << attr.value() << std::endl;
				}
			    }
			 else if (!strcmp(attr.name(),"TargetType")) {
				target_type  = std::strtol(attr.value(), &temp, 10);
				if (*temp != '\0') {
				    std::cout << "Actor::PostInit: Failed to post-initialize: Error reading attribute for " << attr.name() << " Value: " << attr.value() << std::endl;
				}
			    }
			}
		}
	}
    }
	if (!use_vertexarray) {
	    sf::Vector2f pos = position;
	    sf::FloatRect bound = sf::FloatRect(pos.x, pos.y, size.x, size.y);
	    if (pos.x < 0 || pos.y < 0) {
		bool new_position = false;
		bool conflict = false;
		while (!new_position) {
		    pos.x = rand() % (int)(Configuration::getWindowWidth() - size.x * size.x);
		    pos.y = rand() % (int)(Configuration::getWindowHeight() - size.y * size.y);
			//std::cout << pos.x << " " << pos.y << std::endl;
		    bound = sf::FloatRect(pos.x, pos.y, size.x, size.y);
		    std::vector<StrongActorPtr>::iterator it_all;
		    for (it_all = LevelView::actorList.begin(); it_all != LevelView::actorList.end(); it_all++) {
		        if ((*it_all)->intersects(bound) != NULL) {
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
		start_position = position;
		initial_position = position;
		//if (id == "Player")
		//	std::cout << position.x << " int " << position.y << std::endl;
		boundary.clear();
	    boundary.push_back(new sf::FloatRect(position.x, position.y, size.x, size.y));
		if (renderToGameView) {
			for (int i = 0; i < num_directions; i++) {
				if (!sprite_filename[i].empty())
		    			sprite_texture[i].loadFromFile(("./assets/sprites/" + sprite_filename[i]).c_str());
				else
					sprite_texture[i].loadFromFile(("./assets/sprites/" + sprite_filename[0]).c_str());
		    		sprite[i] = sf::Sprite(sprite_texture[i], sf::IntRect(0, 0, (sprite_texture[i].getSize()).x, (sprite_texture[i].getSize()).y));
		   		sprite[i].setScale(size.x/(sprite_texture[i].getSize()).x, size.y/(sprite_texture[i].getSize()).y);
		    		sprite[i].setPosition(position);
			}
		}
		if (renderToMinimap) {
			spriteMinimap_texture.loadFromFile(("./assets/sprites/" + spriteMinimap_filename).c_str());
			sprite_minimap = sf::Sprite(spriteMinimap_texture, sf::IntRect(0, 0, (spriteMinimap_texture.getSize()).x, (spriteMinimap_texture.getSize()).y));
			sprite_minimap.setScale(10.0 * size.x/(spriteMinimap_texture.getSize()).x, 10.0 * size.y/(spriteMinimap_texture.getSize()).y);
			//sprite_minimap.setPosition(position);
			setMinimapSpritePosition(position);
		}

		sprite_idx = 0;
	}
	else {
		for (int i = 0; i < num_directions; i++)
			if (!sprite_filename[i].empty())
				sprite_texture[i].loadFromFile(("./assets/backgrounds/" + sprite_filename[i]).c_str());
			else
				sprite_texture[i].loadFromFile(("./assets/backgrounds/" + sprite_filename[0]).c_str());
			
	}
	//if (id == "Player")
	//	std::cout << position.x << " final " << position.y << std::endl;
	
}

void Actor::PostInit(void) {
	for (ActorComponents::iterator it = components.begin(); it != components.end(); ++it)
		(it->second)->PostInit();
}
/** Moves the actor a certain distance based on the current game time
 ** time: current game time
 ** Prevents actors from moving past the top and bottom boundaries
 ** if the obstacle pointer is set (by a physics component o fthe actor or another actor after contact) it prevents the actors from moving into each other
 **/
    void Actor::move(sf::Vector2f next_pos, sf::Vector2f direction) {
       //Move Actor
	if (direction.x < 0)
		sprite_idx = 2;
	else if (direction.x > 0)
		sprite_idx = 3;
	else if (direction.y < 0)
		sprite_idx = 0;
	else if (direction.y > 0)
		sprite_idx = 1;
        sf::Vector2f p = next_pos;

	
        this->setPosition(p);
        
    }


/** Moves the actor a certain distance based on the current game time
 ** time: current game time
 ** Prevents actors from moving past the top and bottom boundaries
 ** if the obstacle pointer is set (by a physics component o fthe actor or another actor after contact) it prevents the actors from moving into each other
 **/
    void Actor::move(float distance, sf::Vector2f direction) {
        //Move Actor
        sf::Vector2f p = this->getPosition() + direction * distance;

	if (direction.x < 0)
		sprite_idx = 2;
	else if (direction.x > 0)
		sprite_idx = 3;
	else if (direction.y < 0)
		sprite_idx = 0;
	else if (direction.y > 0)
		sprite_idx = 1;


        //Get the bounds after movement and check if the movement is allowed
        sf::FloatRect bound_after = sf::FloatRect(p, getSize());
        std::shared_ptr<ActorComponent>     ac;
        std::shared_ptr<PhysicsComponent>   pc;
        ac = components[PhysicsComponent::id];
        pc = std::dynamic_pointer_cast<PhysicsComponent>(ac);
        if (pc->query(bound_after, direction)) {
            // set the position
            this->setPosition(p);
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
void Actor::render(sf::RenderWindow *window, bool minimap) {
    	if (visible) {
		if (use_vertexarray && renderToGameView) {
			window->draw(sprite_vertexarray, &(sprite_texture[0]));
		}
		else if (minimap && renderToMinimap) {
			window->draw(sprite_minimap);
		}
		else if (!minimap && renderToGameView) {
        		window->draw(sprite[sprite_idx]);
		}
		for (ActorComponents::iterator it = components.begin(); it != components.end(); ++it)
			(it->second)->render(window, minimap);
	}
	
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
	if (use_vertexarray) {
		boundary.clear();
		sf::Vector2f pos;
		int width;
		int height;
		for (int i = 3; i < sprite_vertexarray.getVertexCount(); i += 4) {
			pos = sprite_vertexarray[i-3].position;
			width = sprite_vertexarray[i].position.x - pos.x;
			height = sprite_vertexarray[i].position.y - pos.y;
			boundary.push_back(new  sf::FloatRect(pos.x, pos.y, width, height));
		}
	}	
	else {
		*boundary.back() = sf::FloatRect(position.x, position.y, size.x, size.y);
	}
}


// Mutators and accesors
int Actor::getPathType(void) {
	return path_type;
}

int Actor::getTargetType(void) {
	return target_type;
}

sf::Vector2f Actor::getStartPosition(void) {
	return start_position;
}

sf::Vector2f Actor::getInitialPosition(void) {
	return initial_position;
}

void Actor::setStartPosition(sf::Vector2f pos) {
	start_position = pos;
}
/** returns the actors boundary
 **
 **/
std::vector<sf::FloatRect*> Actor::getBoundary(void) {
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
	//std::cout << pos.x << " setting " << pos.y << std::endl;
	position = pos;
	updateBoundary();
	if (renderToGameView) {
		for (int i = 0; i < num_directions; i++) {
			sprite[i].setPosition(position);
		}
	}
	if (renderToMinimap)
		//sprite_minimap.setPosition(position);
		setMinimapSpritePosition(position);
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
	StrongActorPtr other_actor = EventManagerInterface::getActor(e->getSender());
	if (other_actor == NULL)
		return;

	if (event_type == ContactEvent::event_type)
		std::cout << other_actor->getId() << " made contact with " << id << std::endl;
	
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

/** Checks to see if the Actors intersect
 **
 **/
sf::FloatRect* Actor::intersects(StrongActorPtr other_actor) {
	std::vector<sf::FloatRect*> other_boundary = other_actor->getBoundary();
	for (std::vector<sf::FloatRect*>::iterator it = boundary.begin(); it != boundary.end(); ++it)
		for (std::vector<sf::FloatRect*>::iterator other_it = other_boundary.begin(); other_it != other_boundary.end(); ++other_it)
			if ((*it)->intersects(**other_it))
				return *it;
	return NULL;
}

/** Checks to see if the Actor and boundary intersect
 **
 **/
sf::FloatRect* Actor::intersects(sf::FloatRect bound) {
	for (std::vector<sf::FloatRect*>::iterator it = boundary.begin(); it != boundary.end(); ++it)
		if ((*it)->intersects(bound))
			return *it;
	return NULL;
}

/** Checks to see if the Actor contains point
 **
 **/
sf::FloatRect* Actor::contains(sf::Vector2f pnt) {
	for (std::vector<sf::FloatRect*>::iterator it = boundary.begin(); it != boundary.end(); ++it)
		if ((*it)->contains(pnt))
			return *it;
	return NULL;
}

/** returns whether causes damage
 **
 **/
bool Actor::causesDamage(void) {
    return damage;
}

/**
 **
 **/
void Actor::setMinimapSpritePosition(sf::Vector2f pos) {
	if (this->hasComponent(InputComponent::id)) {
		pos.x -= size.x/2 / sprite_minimap.getScale().x;
		pos.y -= size.y/2 / sprite_minimap.getScale().y;
	}

	sprite_minimap.setPosition(pos);
}
