#include "InputComponent.h"
#include "PhysicsComponent.h"
#include "CollectorComponent.h"
#include "AI.h"
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
ComponentId InputComponent::getId(void) {
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
    if (std::find(cardinals, cardinals+4, dir) != cardinals+4)
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
            else {
                std::cout << "InputComponent::Init: Failed to initialize" << std::endl;
                return false;
            }
        }
    }
    return true;
}

/** Final Initilizer
 ** Setups up additional attributes based on game configuration
 **/
void InputComponent::PostInit(void) {

}

/** Updates the component's attributes
 ** time: current game time
 **/
void InputComponent::update(float time) {
    float distance = time * speed;
    // cast base class to derived so we can use derived class methods
    std::shared_ptr<PhysicsComponent>   pc;
    std::shared_ptr<ActorComponent>     ac;

    sf::Vector2f last_direction = this->getDirection();
    sf::Vector2f next_direction;

    if (type == "Artificial") {
        AI ai(owner->getPosition());
        sf::Vector2f flower_pos = ai.findClosestFlower();
        sf::Vector2f player_pos = ai.findPlayer();

        if (flower_pos == sf::Vector2f(-1.f, -1.f)) {
            next_direction = ai.chooseDirection(player_pos);
        }
        else {
            next_direction = ai.chooseDirection(flower_pos);
        }
    }

    else if (type == "Keyboard") {
        // TODO: I'd like to move controls to a configuration file so we could 
        // choose between up/down/left/right, wasd, and hjkl.
        //Reads Input and perform actions
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
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
    }

    this ->setDirection(next_direction);
    owner->move(distance, direction);
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

/** Cleans up after quiting
 **
 **/
void InputComponent::quit(void) {

}
