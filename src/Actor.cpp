#include "Actor.h"
#include "Constants.h" // for window_[width|height]
#include "PhysicsComponent.h"         // for ids
//unique instance id among actors
int Actor::instances = 0;

//Number of directions possible
const int Actor::num_directions = 4;

/** Gets the current actors ID
 **
 **/
std::vector<ActorId> Actor::getId(void) const {
	return id;
}

/**
 *  Replaces actor Id with new Id - necessary for updating crafted flowers
 */
void Actor::replaceId(ActorId newId, ActorId oldId) {
    //std::cout << "Old Id is " << oldId << std::endl;
    for (auto itr = id.begin(); itr != id.end(); itr++) {
	if (*itr == oldId)
		*itr = newId;
		 //std::cout << "Replaced old Id with " << newId << std::endl;
    }
}

/** test for equality: two actors have same id
 **
 **/
bool Actor::operator==(const Actor& rhs) const {
	return this->getId().back() == rhs.getId().back();
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
	direction = sf::Vector2f(0,0);
	animation_scaling = 1.f;
  use_animation = false;
	id.clear();
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
		if (!strcmp(attr.name(),"Type")) {
			id.push_back(elem->name());
			id.push_back(attr.value());
		}
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
		else if (!strcmp(attr.name(),"SpriteDown")) {
			sprite_filename[1] = attr.value();
		}
		else if (!strcmp(attr.name(),"SpriteLeft")) {
			sprite_filename[2] = attr.value();
		}
		else if (!strcmp(attr.name(),"SpriteRight")) {
			sprite_filename[3] = attr.value();
		}
		else if (!strcmp(attr.name(), "FrameCountUp") || !strcmp(attr.name(), "FrameCount")) {
			sprite_frame_count[0] = strtol(attr.value(), &temp, 10);
      use_animation = true;
		}
		else if (!strcmp(attr.name(), "FrameCountDown")) {
			sprite_frame_count[1] = strtol(attr.value(), &temp, 10);
		}
		else if (!strcmp(attr.name(), "FrameCountLeft")) {
			sprite_frame_count[2] = strtol(attr.value(), &temp, 10);
		}
		else if (!strcmp(attr.name(), "FrameCountRight")) {
			sprite_frame_count[3] = strtol(attr.value(), &temp, 10);
		}
		else if (!strcmp(attr.name(), "FrameDuration")) {
			frame_duration = strtol(attr.value(), &temp, 10);
		}
		else if (!strcmp(attr.name(), "AnimationScaling")) {
			animation_scaling = strtof(attr.value(), &temp);
      std::cout << elem->name() << " animation scaling " << animation_scaling << std::endl;
		}
		else if (!strcmp(attr.name(),"SpriteMinimap")) {
			if (!strcmp(attr.value(), ""))
				renderToMinimap = false;
			else {
				renderToMinimap = true;
				spriteMinimap_filename = attr.value();
			}
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
	//Selects a random position to place the object that is unused
	if (!use_vertexarray) {
	    sf::Vector2f pos = position;
	    sf::FloatRect bound = sf::FloatRect(pos.x, pos.y, size.x, size.y);
	    if (pos.x < 0 || pos.y < 0) {
		bool new_position = false;
		bool conflict = false;
		while (!new_position) {
		    pos.x = rand() % (int)(Configuration::getWindowWidth() - size.x * size.x);
		    pos.y = rand() % (int)(Configuration::getWindowHeight() - size.y * size.y);
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
		if (isOfType("Player")) {
			Configuration::setGameViewCenter(sf::Vector2f((position.x + size.x/2.0), (position.y + size.y/2.0)));
		}
		start_position = position;
		initial_position = position;
		boundary.clear();
	    boundary.push_back(new sf::FloatRect(position.x, position.y, size.x, size.y));
		if (renderToGameView) {
			for (int i = 0; i < num_directions; i++) {
				if (!sprite_filename[i].empty())
					sprite_texture[i].loadFromFile(("./assets/sprites/" + sprite_filename[i]).c_str());
				else
					sprite_texture[i].loadFromFile(("./assets/sprites/" + sprite_filename[0]).c_str());
				sprite[i] = sf::Sprite(sprite_texture[i], sf::IntRect(0, 0, (sprite_texture[i].getSize()).x, (sprite_texture[i].getSize()).y));
				if (this->isOfType("Player") || this->isOfType("NPC") || (use_animation && i == 0)) {
					float x = size.x / (sprite_texture[i].getSize().x/sprite_frame_count[i]);
					float y = size.y / sprite_texture[i].getSize().y;
					sprite[i].setScale(animation_scaling*x, animation_scaling*y);
				}
        else {
					sprite[i].setScale(size.x/(sprite_texture[i].getSize()).x, size.y/(sprite_texture[i].getSize()).y);
				}
				sprite[i].setPosition(position);
			}
		}
		if (renderToMinimap) {
			spriteMinimap_texture.loadFromFile(("./assets/sprites/" + spriteMinimap_filename).c_str());
			sprite_minimap = sf::Sprite(spriteMinimap_texture, sf::IntRect(0, 0, (spriteMinimap_texture.getSize()).x, (spriteMinimap_texture.getSize()).y));
			sprite_minimap.setScale(10.0 * size.x/(spriteMinimap_texture.getSize()).x, 10.0 * size.y/(spriteMinimap_texture.getSize()).y);
			setMinimapSpritePosition(position);
		}

		sprite_idx = 0;
	}
	else {
		for (int i = 0; i < num_directions; i++)
			if (!sprite_filename[i].empty()) {
				//sprite_texture[i].loadFromFile(("./assets/backgrounds/" + sprite_filename[i]).c_str());
			}
			else if (!sprite_filename[0].empty()) {
				//sprite_texture[i].loadFromFile(("./assets/backgrounds/" + sprite_filename[0]).c_str());
			}
	}

	if (use_animation) {
		frame_no = 0;
		this->animate();
	}
}

void Actor::PostInit(void) {
	for (ActorComponents::iterator it = components.begin(); it != components.end(); ++it)
		(it->second)->PostInit();
}
/** Moves the actor a certain distance based on the current game time; used by AI
 ** time: current game time
 ** Prevents actors from moving past the top and bottom boundaries
 ** if the obstacle pointer is set (by a physics component o fthe actor or another actor after contact) it prevents the actors from moving into each other
 **/
void Actor::move(sf::Vector2f next_pos, sf::Vector2f dir) {
  if (!this->isOfType("MovingObstacle")) {
    if (dir.x < 0)
      sprite_idx = 2;
    else if (dir.x > 0)
      sprite_idx = 3;
    else if (dir.y < 0)
      sprite_idx = 0;
    else if (dir.y > 0)
      sprite_idx = 1;
  }

	// update the frame number and animate
  if (use_animation) {
    if (sprite_clock.getElapsedTime().asMilliseconds() > frame_duration) {
      frame_no = (frame_no+1) % sprite_frame_count[sprite_idx];

      this->animate();
      sprite_clock.restart();
    }
    else if (direction != dir) {
      frame_no = 0;

      this->animate();
      sprite_clock.restart();
    }
  }

	//Move Actor
	sf::Vector2f p = next_pos;
	this->setPosition(p);
	direction = dir;

	if (isOfType("Player"))
		Configuration::setGameViewCenter(sf::Vector2f((position.x + size.x/2.0), (position.y + size.y/2.0)));

}


/** Moves the actor a certain distance based on the current game time
 ** time: current game time
 ** Prevents actors from moving past the top and bottom boundaries
 ** if the obstacle pointer is set (by a physics component o fthe actor or another actor after contact) it prevents the actors from moving into each other
 **/
void Actor::move(float distance, sf::Vector2f dir) {
	if (dir.x < 0)
		sprite_idx = 2;
	else if (dir.x > 0)
		sprite_idx = 3;
	else if (dir.y < 0)
		sprite_idx = 0;
	else if (dir.y > 0)
		sprite_idx = 1;

	// update the frame number and animate
  if (use_animation) {
    if (sprite_clock.getElapsedTime().asMilliseconds() > frame_duration) {
      frame_no = (frame_no+1) % sprite_frame_count[sprite_idx];

      this->animate();
      sprite_clock.restart();
    }
    else if (direction != dir) {
      frame_no = 0;

      this->animate();
      sprite_clock.restart();
    }
  }

	//Move Actor
	sf::Vector2f p = this->getPosition() + dir * distance;
	direction = dir;

	// disallow movement off the screen
	unsigned width = Configuration::getWindowWidth();
	unsigned height = Configuration::getWindowHeight();

	if (p.x < FLT_EPSILON)
		p.x = FLT_EPSILON;
	if (p.y < FLT_EPSILON)
		p.y = FLT_EPSILON;
	if (p.x > width-size.x)
		p.x = width - size.x;
	if (p.y > height-size.y)
		p.y = height - size.y;

	//Get the bounds after movement and check if the movement is allowed
	sf::FloatRect bound_after = sf::FloatRect(p, getSize() );
	std::shared_ptr<ActorComponent>     ac;
	std::shared_ptr<PhysicsComponent>   pc;
	ac = components[PhysicsComponent::id];
	pc = std::dynamic_pointer_cast<PhysicsComponent>(ac);
	if (pc->query(bound_after, dir)) {
		// set the position
		this->setPosition(p);
		Configuration::setGameViewCenter(sf::Vector2f((position.x + size.x/2.0), (position.y + size.y/2.0)));
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
	for (ActorComponents::iterator it = components.begin(); it != components.end(); ++it)
		(it->second)->render(window, minimap);
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
	float minimize = .25;
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
		if (isOfType("Obstacle") && !isOfType("Cloud"))
			*boundary.back() = sf::FloatRect(position.x + size.x * minimize, position.y + size.y * minimize, size.x * (1 - minimize), size.y * (1 - minimize));
		else
			*boundary.back() = sf::FloatRect(position.x, position.y, size.x, size.y);
	}
}


// Mutators and accesors

//Returns the path type
int Actor::getPathType(void) {
	return path_type;
}

//Returns the target path type
int Actor::getTargetType(void) {
	return target_type;
}

//Returns the current player path start position
sf::Vector2f Actor::getStartPosition(void) {
	return start_position;
}

//Returns the player's initial game position
sf::Vector2f Actor::getInitialPosition(void) {
	return initial_position;
}

//Sets the current player path start position
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

	//if (event_type == ContactEvent::event_type)
	//	std::cout << other_actor->getId() << " made contact with " << id << std::endl;
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
sf::FloatRect* Actor::intersects(StrongActorPtr other_actor, bool use_border) {
	if (!use_border)
		border = 0;
	std::vector<sf::FloatRect*> other_boundary = other_actor->getBoundary();
	for (std::vector<sf::FloatRect*>::iterator it = boundary.begin(); it != boundary.end(); ++it) {
		sf::FloatRect my_border = sf::FloatRect((*it)->left - border, (*it)->top - border, (*it)->width + 2*border, (*it)->height + 2*border);
		for (std::vector<sf::FloatRect*>::iterator other_it = other_boundary.begin(); other_it != other_boundary.end(); ++other_it)
			if ((my_border).intersects(**other_it))
				return (*it);
	}
	return NULL;
}

/** Checks to see if the Actor and boundary intersect
 **
 **/
sf::FloatRect* Actor::intersects(sf::FloatRect bound, bool use_border) {
	if (!use_border)
		border = 0;
	for (std::vector<sf::FloatRect*>::iterator it = boundary.begin(); it != boundary.end(); ++it) {
		sf::FloatRect my_border = sf::FloatRect((*it)->left - border, (*it)->top - border, (*it)->width + 2*border, (*it)->height + 2*border);
		if ((my_border).intersects(bound))
			return (*it);
	}
	return NULL;
}

//Returns the players current direction
sf::Vector2f Actor::getDirection(void) {
	return direction;
}

//Sets the players current direction
void Actor::setDirection(sf::Vector2f dir) {
	direction = dir;
}

/** Checks to see if the Actor contains point
 **
 **/
sf::FloatRect* Actor::contains(sf::Vector2f pnt, bool use_border) {
	if (!use_border)
		border = 0;
	for (std::vector<sf::FloatRect*>::iterator it = boundary.begin(); it != boundary.end(); ++it) {
		sf::FloatRect my_border = sf::FloatRect((*it)->left - border, (*it)->top - border, (*it)->width + 2*border, (*it)->height + 2*border);
		if ((my_border).contains(pnt))
			return *it;
	}
	return NULL;
}

/** returns whether causes damage
 **
 **/
bool Actor::causesDamage(void) {
    return damage;
}

/** Sets the minimap sprite position
 **
 **/
void Actor::setMinimapSpritePosition(sf::Vector2f pos) {
	sprite_minimap.setPosition(pos);
	if (this->isOfType("Player") || this->isOfType("Homer") || this->isOfType("WaterFlower") || this->isOfType("EarthFlower") || this->isOfType("EarthFlower") || this->isOfType("FireFlower")) {
		sf::FloatRect bounds = sprite_minimap.getGlobalBounds();
		float x = bounds.width/2; // sprite_minimap.getScale().x;
		float y = bounds.height/2; // sprite_minimap.getScale().y;
		sprite_minimap.move(-x, -y);
	}
}

//CHeck if actor is of given type
bool Actor::isOfType(ActorId type) {
	for (auto itr = id.begin(); itr != id.end(); itr++) {
		if (*itr == type)
			return true;
	}
	return false;
}

/** Update the animation frame.
 **
 **/
void Actor::animate() {
	// sanity check
	if (!use_animation) {
		return;
	}

	// determine the bounds of the frame
	int top  = 0;
	int width = sprite_texture[sprite_idx].getSize().x / sprite_frame_count[sprite_idx];
	int left = frame_no * width;
	int height = sprite_texture[sprite_idx].getSize().y;

	// set the frame
	sprite[sprite_idx].setTextureRect(sf::IntRect(left, top, width, height));
}
