#include "InputComponent.h"
#include <algorithm> // std::find()
#include <cstdlib>   // std::rand()

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
InputComponent::InputComponent(void) :
  NORTH(sf::Vector2f(0.f, -1.f)),     // these would be static const members
  SOUTH(sf::Vector2f(0.f, 1.f)),      // except that prevents the use of std::find()
  EAST(sf::Vector2f(1.f, 0.f)),       // which was kind of the whole point
  WEST(sf::Vector2f(-1.f, 0.f)),
  cardinals{ NORTH, SOUTH, EAST, WEST }
{
	instance = instances;
}

/** Initializer
 ** elem : node pointing to section of XML configuration holding more attribute defaults to setup
 ** Sets up additional attribute defaults
**/
bool InputComponent::Init(pugi::xml_node* elem) {	
	//Iterate over the component's attributes
	for (pugi::xml_node tool = elem->first_child(); tool; tool = tool.next_sibling()) {
		for (pugi::xml_attribute attr = tool.first_attribute(); attr; attr = attr.next_attribute()) {		
			if (!strcmp(attr.name(),"Type"))
				type = attr.value();
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
	float distance = time * velocity;

	if (type == "Artificial") {
		// TODO: this should be done with path finding
		this->setDirection(cardinals[std::rand() % 4]);
		owner->move(distance, direction);
	}

	else if (type == "Keyboard") {
		// TODO: I'd like to move controls to a configuration file so we could 
		// choose between up/down/left/right, wasd, and hjkl.
		//Reads Input and perform actions
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
			this->setDirection(NORTH);
			owner->move(distance, direction);
		} 
		else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
			this->setDirection(SOUTH);
			owner->move(distance, direction);
		} 
		else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
			this->setDirection(WEST);
			owner->move(distance, direction);
		} 
		else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
			this->setDirection(EAST);
			owner->move(distance, direction);
		}
	}
}

/** Receives event when the actor is being contacted by another actor and responds by accordingly
 **
**/
void InputComponent::update(EventInterfacePtr e) {
  // get pointer to the other actor involved in the contact
  EventType event_type = e->getEventType();
  StrongActorPtr other_actor = LevelView::actors[e->getSender()];

  // is the player making contact?
  if (event_type == ContactEvent::event_type && type == "Keyboard") {

    // is the other actor an NPC or an obstacle?
    if (other_actor->hasComponent(CollectorComponent::id) || other_actor->components.empty())
    {
      // cast base class to derived so we can use derived class methods
      std::shared_ptr<CollectorComponent> cc;
      std::shared_ptr<ActorComponent>     ac;

      ac = owner->components[CollectorComponent::id];
      cc = std::dynamic_pointer_cast<CollectorComponent>(ac);

      int vases = cc->getVases();
      std::cerr << "vases before: " << vases << std::endl;
      cc->setVases(vases-1);

      vases = cc->getVases();
      std::cerr << "vases after: " << vases << std::endl;
    }
  }
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
