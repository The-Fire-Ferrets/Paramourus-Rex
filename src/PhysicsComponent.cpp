#include "PhysicsComponent.h"

//Unique instance id among instances of the same component
int PhysicsComponent::instances = -1;
//Unique component id
ComponentId PhysicsComponent::id = "PhysicsComponent";
//Registers component with factory
const bool PhysicsComponent::registered = ActorFactory::registerComponent(&id, &create);


/** Returns the id shared by all components of this type
 **
 **/
ComponentId PhysicsComponent::getId(void) const {
    return id;
}

/** Returns a reference to the components constructor
 **
 **/
ActorComponent* PhysicsComponent::create() { 
    instances++;
    return new PhysicsComponent();
}

/** Constructor
 ** Sets up unique instance ID
 **/
PhysicsComponent::PhysicsComponent(void) {
    instance = instances;
	direction_bit = 0;
	use_vision_boundary = false;
	inVision = false;
	first_run = 0;
}

/** Destructor
 **
 **/
PhysicsComponent::~PhysicsComponent(void) {	
	last_actors.clear();
}

/** Initilizer
 ** elem : node pointing to section of XML configuration holding more attribute defaults to setup
 ** Sets up additional attribute defaults
 **/
bool PhysicsComponent::Init(pugi::xml_node* elem) {
	char* temp;
    for (pugi::xml_node tool = elem->first_child(); tool; tool = tool.next_sibling()) {
        for (pugi::xml_attribute attr = tool.first_attribute(); attr; attr = attr.next_attribute()) {
		//std::cout << tool.name() << " " << attr.name() << " " << attr.value() << std::endl;
            if (!strcmp(attr.name(),"Type"))
                type = attr.value();
		 else if (!strcmp(tool.name(), "VisionBoundary"), !strcmp(attr.name(),"X")) {
			if (!strcmp(attr.value(), ""))
				vision_boundary_position.x = 0;
			else {
				vision_boundary_position.x  = std::strtol(attr.value(), &temp, 10);
				use_vision_boundary = true;
				if (*temp != '\0') {
				    std::cout << "PhysicsComponent::Init: Failed to post-initialize: Error reading attribute for " << attr.name() << " Value: " << attr.value() << std::endl;
				}
			}
		    }
		else if (!strcmp(tool.name(), "VisionBoundary"), !strcmp(attr.name(),"Y")) {
			if (!strcmp(attr.value(), ""))
				vision_boundary_position.y = 0;
			else {
				vision_boundary_position.y  = std::strtol(attr.value(), &temp, 10);
				if (*temp != '\0') {
				    std::cout << "PhysicsComponent::Init: Failed to post-initialize: Error reading attribute for " << attr.name() << " Value: " << attr.value() << std::endl;
				}
			}
		    }
		else if (!strcmp(tool.name(), "VisionBoundary"), !strcmp(attr.name(),"Width")) {
			if (!strcmp(attr.value(), ""))
				vision_boundary_size.x = 0;
			else {
				vision_boundary_size.x  = std::strtol(attr.value(), &temp, 10);
				if (*temp != '\0') {
				    std::cout << "PhysicsComponent::Init: Failed to post-initialize: Error reading attribute for " << attr.name() << " Value: " << attr.value() << std::endl;
				}
			}
		    }
		else if (!strcmp(tool.name(), "VisionBoundary"), !strcmp(attr.name(),"Height")) {
			if (!strcmp(attr.value(), ""))
				vision_boundary_size.y = 0;
			else {
				vision_boundary_size.y  = std::strtol(attr.value(), &temp, 10);
				if (*temp != '\0') {
				    std::cout << "PhysicsComponent::Init: Failed to post-initialize: Error reading attribute for " << attr.name() << " Value: " << attr.value() << std::endl;
				}
			}
		    }
        }
    }
	if (use_vision_boundary) {
		vision_boundary_texture.loadFromFile("./assets/backgrounds/Level0.png");
		vision_boundary_sprite = sf::Sprite(vision_boundary_texture);
		//std::cout << "HERE" << std::endl;
	}
	
    return true;	
}

/** Final Initilizer
 ** Setups up additional attributes based on game configuration
 **/
bool PhysicsComponent::PostInit(pugi::xml_node* elem) {
	if (elem != NULL)
	Init(elem);
	return true;
}

bool PhysicsComponent::PostInit(void) {

}

/** Checks to see if the bound of two actor intersect and sends ContactEvent to the event manager, keeps track of last actor made contact with
 ** time: current game time
 **/
void PhysicsComponent::update(float time) {
	//Checks vision boundary to see if player is seen
	if (use_vision_boundary) {	
		vision_boundary = sf::IntRect(0, 0, (int) (vision_boundary_size.x), (int) (vision_boundary_size.y));
		vision_boundary_sprite.setTextureRect(vision_boundary);
		vision_boundary_sprite.setPosition(owner->getPosition().x + vision_boundary_position.x, owner->getPosition().y + vision_boundary_position.y);
		if (LevelView::player->intersects(vision_boundary_sprite.getGlobalBounds()) && !inVision) {
			inVision = true;
			//std::cout << "Player Seen" << std::endl;
			Pathfinder::changeVision(owner->getInitialPosition());
			vision_timer.restart();
			LevelView::inVision++;
		}
		else if (inVision && vision_timer.getElapsedTime().asSeconds() > 15) {
			inVision = false;
			//std::cout << "Player Seen" << std::endl;
			Pathfinder::changeVision(owner->getInitialPosition());
			LevelView::inVision--;
		}
			 
	}
    bool madeContact = false;
    std::vector<StrongActorPtr>::iterator it_all;
    for (it_all =  LevelView::actorList.begin(); it_all !=  LevelView::actorList.end(); it_all++) {
	bool madeContactPrev = false;
        StrongActorPtr other_actor = *it_all;
        if (owner->getInstance() != other_actor->getInstance()) {
            //Checks to see if actor was in the last contact episode
            std::vector<StrongActorPtr>::iterator it;
		std::vector<std::pair<sf::FloatRect*, int>>::iterator it_b = last_boundaries.end();
            for (it = last_actors.begin(); it != last_actors.end(); it++) {
                if (*it == other_actor) {
			  for (it_b = last_boundaries.begin(); it_b != last_boundaries.end(); it_b++) {
				if (other_actor->intersects(*(it_b->first)) != NULL)
					break;
				}
			madeContactPrev = true;	
			it == last_actors.end();				
                    break;
                }
            }
	    sf::FloatRect* bound;
            if ((bound = other_actor->intersects(owner)) != NULL) {
                madeContact = true;
                if (it == last_actors.end()) {
			if (!madeContactPrev) {
                    if (!EventManagerInterface::get()->queueEvent(new ContactEvent(time, owner->getInstance(), other_actor->getInstance())) )
                        std::cout << "PhysicsComponent::update: Unable to queue event" << std::endl;
                    last_actors.push_back(other_actor);
			madeContactPrev = false;
			}
			if (other_actor->hasAttribute("Opaque")) {
			if (owner->getId() == "Player")
			//std::cout << "Added new boundary!" << std::endl;
			last_boundaries.push_back(std::pair<sf::FloatRect*, int>(bound, setDirectionBit(0, collisionSide(*bound) ) ) );
			}
                }
            }
            else {
		if(it != last_actors.end())
                last_actors.erase(it);
		if(it_b != last_boundaries.end())
                last_boundaries.erase(it_b);
            }
        }
    }
    if (!madeContact) {
        last_actors.clear();
	last_boundaries.clear();
	}

}


sf::Vector2f PhysicsComponent::collisionSide(sf::FloatRect other_bounds) {
	sf::FloatRect* owner_bounds = (owner->getBoundary()).back();
	int owner_left = owner_bounds->left;
	int owner_right = owner_bounds->left + owner_bounds->width;
	int owner_top = owner_bounds->top;
	int owner_bottom = owner_bounds->top + owner_bounds->height;

	int other_left = other_bounds.left;
	int other_right = other_bounds.left + other_bounds.width;
	int other_top = other_bounds.top;
	int other_bottom = other_bounds.top + other_bounds.height;

	sf::Vector2f dir = owner->getDirection();

	int right_diff = abs(owner_right - other_left);
	int left_diff = abs(owner_left - other_right);
	int top_diff = abs(owner_top - other_bottom);
	int bottom_diff = abs(owner_bottom - other_top);

	//std::cout << "Right: " << right_diff << " Left: " << left_diff << " Top: " << top_diff << " Bottom: " << bottom_diff << std::endl;
	if (dir.x == 1 && right_diff < top_diff && right_diff < bottom_diff )
		return sf::Vector2f(1,0);
	if (dir.x == -1 && left_diff < top_diff && left_diff < bottom_diff )
		return sf::Vector2f(-1,0);
	if (dir.y == -1 && top_diff < left_diff && top_diff < right_diff )
		return sf::Vector2f(0,-1);
	if (dir.y == 1 && bottom_diff < left_diff && bottom_diff < right_diff )
		return sf::Vector2f(0,1);
	return sf::Vector2f(0,0);
}
/** Receives event when the actor is being contacted by another actor and responds by accordingly
 **
 **/
void PhysicsComponent::update(EventInterfacePtr e) {

}
/** Reset the component
 **
 **/
void PhysicsComponent::reset(void) {

}

/** Restarts the component
 **
 **/
void PhysicsComponent::restart(void) {

}

/** Cleans up component after quiting
 **
 **/
                void PhysicsComponent::quit(void) {

                }

/** Check for intersections between this compoenent's owner
 ** and other actor's.
 **/
bool PhysicsComponent::query(sf::FloatRect bound, sf::Vector2f dir) {
	sf::FloatRect* other_bound;
	for (auto it = last_boundaries.begin(); it != last_boundaries.end(); it++) {
		other_bound = it->first;
		bool val;
		if (other_bound-> intersects(bound) && (val = getDirectionBit(it->second, dir))) {
			std::cout << "Direction: " << dir.x << " " << dir.y << " val: " << val << std::endl;
			
			return false;
		}
		std::cout << "Direction: " << dir.x << " " << dir.y << " val: " << val << std::endl;
	}
	return true;
}
/*bool PhysicsComponent::query(sf::FloatRect bound, sf::Vector2f dir) {
    // is the owner currently in another actor's bounding box?
	int border = 10;
	//sf::FloatRect boundwborder = sf::FloatRect(bound.left - border, bound.top - border, bound.width + 2*border, bound.height + 2*border);
	bound = sf::FloatRect(bound.left - border, bound.top - border, bound.width + 2*border, bound.height + 2*border);
	sf::FloatRect* other_bound;
    for (auto it = last_boundaries.begin(); it != last_boundaries.end(); it++) {
	other_bound = it->first;
	
        if ((it->first)->intersects(bound)) {
            setDirectionBit(dir);

		if (first_run == 0) {
			std::cout << dir.x << " " << dir.y << std::endl;
			std::cout << "Player Bounds: " << bound.left << " " << bound.top << " " << bound.width << " " << bound.height << std::endl;
			std::cout << "Wall Bounds: " << other_bound->left << " " << other_bound->top << " " << other_bound->width << " " << other_bound->height << std::endl;
		}
		first_run++;
            //If caused contact but on an edge, ignore it and only return false if a majoriry of the player is making contact
            //Allows for smoother movement and continued movement

            if (dir.x == 1) {
                if (other_bound->contains(sf::Vector2f(bound.left + bound.width, bound.top  + bound.height / 4)))
                    return false;
                else if (other_bound->contains(sf::Vector2f(bound.left + bound.width, bound.top + bound.height / 2)))      
                    return false;
                else if (other_bound->contains(sf::Vector2f(bound.left + bound.width, bound.top + 3* bound.height / 4)))
                    return false;
            }	
            if (dir.x == -1) {
                if (other_bound->contains(sf::Vector2f(bound.left, bound.top  + bound.height / 4)))    
                    return false;
                else if (other_bound->contains(sf::Vector2f(bound.left, bound.top + bound.height / 2)))    
                    return false;
                else if (other_bound->contains(sf::Vector2f(bound.left, bound.top + 3*bound.height / 4)))    
                    return false;
            }	
            if (dir.y == 1) {
                if (other_bound->contains(sf::Vector2f(bound.left  + bound.width / 4, bound.top + bound.height)))     
                    return false;
                else if (other_bound->contains(sf::Vector2f(bound.left + bound.width/2, bound.top + bound.height)))     
                    return false;
                else if (other_bound->contains(sf::Vector2f(bound.left + 3 * bound.width / 4, bound.top + bound.height)))     
                    return false;
            }	
            if (dir.y == -1) {
                if (other_bound->contains(sf::Vector2f(bound.left  + bound.width / 4, bound.top)))     
                    return false;
                else if (other_bound->contains(sf::Vector2f(bound.left + bound.width/2, bound.top)))     
                    return false;
                else if (other_bound->contains(sf::Vector2f(bound.left + 3* bound.width / 4, bound.top)))     
                    return false;		
            }
        }
    }
   flipDirectionBit(dir);
    // if not, it is okay to move in the direction we want
    return true;
}*/

/** Renders component
 ** window: current game render window
 **/
void PhysicsComponent::render(sf::RenderWindow *window, bool minimap) {
	if (use_vision_boundary)
		window->draw(vision_boundary_sprite);
}

/** Sets the value of the given bit
 **
**/
void PhysicsComponent::setDirectionBit(int bit_num) {
	direction_bit = direction_bit | 1<<(bit_num - 1);
}

/** Sets the value of the given bit
 **
**/
int PhysicsComponent::setDirectionBit(int bit, int bit_num) {
	return (bit | 1<<(bit_num - 1));
}

/** Sets the value of the given bit
 **
**/
void PhysicsComponent::setDirectionBit(sf::Vector2f dir) {
	if (dir.x == 1) {
		setDirectionBit(1);
	}
	else if (dir.x == -1) {
		setDirectionBit(2);
	}
	else if (dir.y == -1) {
		setDirectionBit(3);
	}
	else if (dir.y == 1) {
		setDirectionBit(4);
	}
}

/** Sets the value of the given bit
 **
**/
int PhysicsComponent::setDirectionBit(int bit, sf::Vector2f dir) {
	if (dir.x == 1) {
		bit = setDirectionBit(bit, 1);
	}
	if (dir.x == -1) {
		bit = setDirectionBit(bit, 2);
	}
	if (dir.y == -1) {
		bit = setDirectionBit(bit, 3);
	}
	if (dir.y == 1) {
		bit = setDirectionBit(bit, 4);
	}
	return bit;
}

/** Returns the value of the given bit
 **
**/
bool PhysicsComponent::getDirectionBit(int bit_num) {
	return direction_bit & 1<<(bit_num - 1);
}

/** Returns the value of the given bit
 **
**/
bool PhysicsComponent::getDirectionBit(int bit, int bit_num) {
	return bit & 1<<(bit_num - 1);
}

/** Returns the value of the given bit
 **
**/
bool PhysicsComponent::getDirectionBit(sf::Vector2f dir) {
	if (dir.x == 1) {
		return getDirectionBit(1);
	}
	else if (dir.x == -1) {
		return getDirectionBit(2);
	}
	else if (dir.y == -1) {
		return getDirectionBit(3);
	}
	else if (dir.y == 1) {
		return getDirectionBit(4);
	}
	return false;
}

/** Returns the value of the given bit
 **
**/
bool PhysicsComponent::getDirectionBit(int bit, sf::Vector2f dir) {
	if (dir.x == 1 && dir.y == 1) {
		return getDirectionBit(bit, 1) || getDirectionBit(bit, 4);
	}

	if (dir.x == 1 && dir.y == -1) {
		return getDirectionBit(bit, 1) ||  getDirectionBit(bit, 3);
	}

	if (dir.x == -1 && dir.y == -1) {
		return getDirectionBit(bit, 2) ||  getDirectionBit(bit, 3);
	}

	if (dir.x == -1 && dir.y == 1) {
		return getDirectionBit(bit, 2) ||  getDirectionBit(bit, 4);
	}

	if (dir.x == 1) {
		return getDirectionBit(bit, 1);
	}
	if (dir.x == -1) {
		return getDirectionBit(bit, 2);
	}
	if (dir.y == -1) {
		return getDirectionBit(bit, 3);
	}
	if (dir.y == 1) {
		return getDirectionBit(bit, 4);
	}
	return false;
}

/** Flips the given bit
 **
**/
void PhysicsComponent::flipDirectionBit(int bit_num) {
	direction_bit = direction_bit ^ 1<<(bit_num - 1);
}

/** Flips the given bit
 **
**/
int PhysicsComponent::flipDirectionBit(int bit, int bit_num) {
	return bit ^ 1<<(bit_num - 1);
}

/** flips the value of the given bit
 **
**/
void PhysicsComponent::flipDirectionBit(sf::Vector2f dir) {
	if (dir.x == 1) {
		flipDirectionBit(1);
	}
	if (dir.x == -1) {
		flipDirectionBit(2);
	}
	if (dir.y == -1) {
		flipDirectionBit(3);
	}
	if (dir.y == 1) {
		flipDirectionBit(4);
	}
}


/** flips the value of the given bit
 **
**/
int PhysicsComponent::flipDirectionBit(int bit, sf::Vector2f dir) {
	if (dir.x == 1) {
		bit = flipDirectionBit(bit, 1);
	}
	if (dir.x == -1) {
		bit = flipDirectionBit(bit, 2);
	}
	if (dir.y == -1) {
		bit = flipDirectionBit(bit, 3);
	}
	if (dir.y == 1) {
		bit = flipDirectionBit(bit, 4);
	}
	return bit;
}
/** Resets all directions to 0
 **
**/
void PhysicsComponent::resetDirectionBit() {
	direction_bit = 0;
}
