#include "InputComponent.h"
#include "PhysicsComponent.h"
#include "CollectorComponent.h"
#include <algorithm> // std::find()

#include <SFML/Window/Keyboard.hpp> // sf::Keyboard, sf::Key
//Unique instance id among instances of the same component
int InputComponent::instances = -1;
//Unique component id
ComponentId InputComponent::id = "InputComponent";
//Registers component with factory
const bool InputComponent::registered = ActorFactory::registerComponent(&id, &create);

/** Returns the id shared by all components of this type
 **
 **/
ComponentId InputComponent::getId(void) const {
    return id;
}

/** Returns the direction in which the actor is currently moving
 **
 **/
sf::Vector2f InputComponent::getDirection() const {
    return direction;
}

/** Sets the direction in which the actor is currently moving
 **
 **/
void InputComponent::setDirection(const sf::Vector2f& dir) {
    // reset direction only if it is a cardinal direction
	//if (std::find(cardinals, cardinals+4, dir) != cardinals+4)    
	direction = dir;
}

/** Returns a reference to the components constructor
 **
 **/
ActorComponent* InputComponent::create() { 
    //update the instance count
    instances++;
    return new InputComponent();
}

/** Constructor
 ** Sets up unique instance ID
 **/
InputComponent::InputComponent(void)
{
    instance = instances;
	last_framerate = 0;
	first_postinit = false;
	fake_pos = sf::Vector2f(0,0);
	fake_dir = sf::Vector2f(0,0);
}

/** Destructor
 **
 **/
InputComponent::~InputComponent() {

}

/** Initializer
 ** elem : node pointing to section of XML configuration holding more attribute defaults to setup
 ** Sets up additional attribute defaults
 **/
bool InputComponent::Init(pugi::xml_node* elem) {	
    //Iterate over the component's attributes
    char * temp;
    for (pugi::xml_node tool = elem->first_child(); tool; tool = tool.next_sibling()) {
        for (pugi::xml_attribute attr = tool.first_attribute(); attr; attr = attr.next_attribute()) {		
            if (!strcmp(attr.name(),"Type"))
                type = attr.value();
            else if (!strcmp(attr.name(), "Speed")) {
                speed = std::strtof(attr.value(), &temp);
            }
        }
    }
    return true;
}

/** Final Initilizer
 ** Setups up additional attributes based on game configuration
 **/
bool InputComponent::PostInit(pugi::xml_node* elem) {
	if (elem != NULL)
	Init(elem);
	first_postinit = true;
	return true;
}

bool InputComponent::PostInit(void) {

}

/** Updates the component's attributes
 ** time: current game time
 **/
void InputComponent::update(float time) {
    float distance = ((time - last_framerate));
	if (distance < 0)
		distance = Configuration::getFrameRate() + distance;
	last_framerate = time;
	distance *= speed;
    // cast base class to derived so we can use derived class methods
    std::shared_ptr<PhysicsComponent>   pc;
    std::shared_ptr<ActorComponent>     ac;

    sf::Vector2f next_direction = owner->getDirection();;

    if (type == "Artificial") {
	sf::Vector2f next_pos;
	sf::Vector2f curr_pos;
	//if (fake_pos == sf::Vector2f(0,0)) {
		next_pos = owner->getPosition();
		curr_pos = owner->getPosition();
	/*}
	else {
		next_pos = fake_pos;
		curr_pos = fake_pos;
	}*/

	/*if (distance > 0) {
		std::cout << distance << std::endl;
		std::cout << next_pos.x << " " << next_pos.y << std::endl;
	}*/
	//std::cout << "Input1" << std::endl;
	bool start_changed = Pathfinder::getNextPosition(distance, owner->getInitialPosition(), owner->getStartPosition(), curr_pos, &next_pos, &next_direction);

	if (start_changed)
		owner->setStartPosition(owner->getPosition());

	/*sf::FloatRect bound_after = sf::FloatRect(next_pos, owner->getSize() );
        std::shared_ptr<ActorComponent>     ac;
        std::shared_ptr<PhysicsComponent>   pc;
        ac = owner->components[PhysicsComponent::id];
        pc = std::dynamic_pointer_cast<PhysicsComponent>(ac);
	if (pc->query(bound_after, next_direction)) {*/
		owner->move(next_pos, next_direction);
		/*fake_pos = sf::Vector2f(0,0);
		fake_dir = sf::Vector2f(0,0);
	}
	else {
		fake_pos = next_pos;
		fake_dir = next_direction;
	}*/

    }

    else if (type == "Keyboard") {
		// TODO: I'd like to move controls to a configuration file so we could 
		// choose between up/down/left/right, wasd, and hjkl.
		//Reads Input and perform actions
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up) && sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
		    next_direction = NORTHEAST;
		    //distance *= cos(45);
		}   
		else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up) && sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
		    next_direction = NORTHWEST;
		    //distance *= cos(45);
		}      
		else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down) && sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
		    next_direction = SOUTHEAST;
		    //distance *= cos(45);
		}    
		else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down) && sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
		    next_direction = SOUTHWEST;
		    //distance *= cos(45);
		}        
		else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
		    next_direction = NORTH;
		} 
		else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
		    next_direction = SOUTH;
		} 
		else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
		    next_direction = WEST;
		} 
		else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
		    next_direction = EAST;
		}
		else return; // No input provided.
		
		 this ->setDirection(next_direction);
	    	owner->move(distance, direction);
    }
}

/** Receives event when the actor is being contacted by another actor and responds by accordingly
 **
 **/
void InputComponent::update(EventInterfacePtr e) {

}

/** Reset the component
 **
 **/
                void InputComponent::reset(void) {

                }

/** Restart the component
 **
 **/
void InputComponent::restart(void) {

}

/** Renders component
 ** window: current game render window
 **/
void InputComponent::render(sf::RenderWindow *window, bool minimap) {
	
}

/** Cleans up after quiting
 **
 **/
void InputComponent::quit(void) {

}
