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
    last_dir = sf::Vector2f(0,0);
	direction_bit = 0;
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
bool PhysicsComponent::Init(pugi::xml_node* elem) {;
    for (pugi::xml_node tool = elem->first_child(); tool; tool = tool.next_sibling()) {
        for (pugi::xml_attribute attr = tool.first_attribute(); attr; attr = attr.next_attribute()) {
            if (!strcmp(attr.name(),"Type"))
                type = attr.value();
        }
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

/** Checks to see if the bound of two actor intersect and sends ContactEvent to the event manager, keeps track of last actor made contact with
 ** time: current game time
 **/
void PhysicsComponent::update(float time) {
    bool madeContact = false;
    std::vector<StrongActorPtr>::iterator it_all;
    for (it_all = LevelView::actorList.begin(); it_all != LevelView::actorList.end(); it_all++) {
	bool madeContactPrev = false;
        StrongActorPtr other_actor = *it_all;
        if (owner->getInstance() != other_actor->getInstance()) {
            //Checks to see if actor was in the last contact episode
            std::vector<StrongActorPtr>::iterator it;
		std::vector<sf::FloatRect*>::iterator it_b = last_boundaries.end();
            for (it = last_actors.begin(); it != last_actors.end(); it++) {
                if (*it == other_actor) {
			  for (it_b = last_boundaries.begin(); it_b != last_boundaries.end(); it_b++) {
				if (other_actor->intersects(**it_b) != NULL)
					break;
				}
			madeContactPrev = true;	
			it == last_actors.end();				
                    break;
                }
            }
	    sf::FloatRect* bound;
            if ((bound = owner->intersects(other_actor)) != NULL) {
                madeContact = true;
                if (it == last_actors.end()) {
			if (!madeContactPrev) {
                    if (!EventManagerInterface::get()->queueEvent(new ContactEvent(time, owner->getInstance(), other_actor->getInstance())) )
                        std::cout << "PhysicsComponent::update: Unable to queue event" << std::endl;
                    last_actors.push_back(other_actor);
			madeContactPrev = false;
			}
			last_boundaries.push_back(bound);
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
    // is the owner currently in another actor's bounding box?
    for (auto it = last_actors.begin(); it != last_actors.end(); it++) {
        if ( (*it)->hasAttribute("Opaque") && (*it)->intersects(bound) != NULL) {
            setDirectionBit(dir);

            //If caused contact but on an edge, ignore it and only return false if a majoriry of the player is making contact
            //Allows for smoother movement and continued movement
            if (dir.x == 1) {
                if ((*it)->contains(sf::Vector2f(bound.left + bound.width, bound.top  + bound.height / 4)) != NULL)
                    return false;
                else if ((*it)->contains(sf::Vector2f(bound.left + bound.width, bound.top + bound.height / 2)) != NULL)      
                    return false;
                else if ((*it)->contains(sf::Vector2f(bound.left + bound.width, bound.top + 3* bound.height / 4)) != NULL)
                    return false;
            }	
            else if (dir.x == -1) {
                if ((*it)->contains(sf::Vector2f(bound.left, bound.top  + bound.height / 4)) != NULL)    
                    return false;
                else if ((*it)->contains(sf::Vector2f(bound.left, bound.top + bound.height / 2)) != NULL)    
                    return false;
                else if ((*it)->contains(sf::Vector2f(bound.left, bound.top + 3*bound.height / 4)) != NULL)    
                    return false;
            }	
            else if (dir.y == 1) {
                if ((*it)->contains(sf::Vector2f(bound.left  + bound.width / 4, bound.top + bound.height)) != NULL)     
                    return false;
                else if ((*it)->contains(sf::Vector2f(bound.left + bound.width/2, bound.top + bound.height)) != NULL)     
                    return false;
                else if ((*it)->contains(sf::Vector2f(bound.left + 3 * bound.width / 4, bound.top + bound.height)) != NULL)     
                    return false;
            }	
            else if (dir.y == -1) {
                if ((*it)->contains(sf::Vector2f(bound.left  + bound.width / 4, bound.top)) != NULL)     
                    return false;
                else if ((*it)->contains(sf::Vector2f(bound.left + bound.width/2, bound.top)) != NULL)     
                    return false;
                else if ((*it)->contains(sf::Vector2f(bound.left + 3* bound.width / 4, bound.top)) != NULL)     
                    return false;		
            }
        }
    }
   flipDirectionBit(dir);
    // if not, it is okay to move in the direction we want
    return true;
}

/** Renders component
 ** window: current game render window
 **/
void PhysicsComponent::render(sf::RenderWindow *window, bool minimap) {
	
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

/** Returns the value of the given bit
 **
**/
bool PhysicsComponent::getDirectionBit(int bit_num) {
	return direction_bit & 1<<(bit_num - 1);
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
/** Flips the given bit
 **
**/
void PhysicsComponent::flipDirectionBit(int bit_num) {
	direction_bit = direction_bit ^ 1<<(bit_num - 1);
}

/** flips the value of the given bit
 **
**/
void PhysicsComponent::flipDirectionBit(sf::Vector2f dir) {
	if (dir.x == 1) {
		flipDirectionBit(1);
	}
	else if (dir.x == -1) {
		flipDirectionBit(2);
	}
	else if (dir.y == -1) {
		flipDirectionBit(3);
	}
	else if (dir.y == 1) {
		flipDirectionBit(4);
	}
}
/** Resets all directions to 0
 **
**/
void PhysicsComponent::resetDirectionBit() {
	direction_bit = 0;
}
