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
ComponentId PhysicsComponent::getId(void) {
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
}

/** Initilizer
 ** elem : node pointing to section of XML configuration holding more attribute defaults to setup
 ** Sets up additional attribute defaults
**/
bool PhysicsComponent::Init(pugi::xml_node* elem) {
	if (debug_mode)
		std::cout << elem->name() << std::endl;
	for (pugi::xml_node tool = elem->first_child(); tool; tool = tool.next_sibling()) {
		for (pugi::xml_attribute attr = tool.first_attribute(); attr; attr = attr.next_attribute()) {
			if (debug_mode)
				std::cout << attr.value() << std::endl;
		}
	}
	
	return true;
	
}

/** Final Initilizer
 ** Setups up additional attributes based on game configuration
**/
void PhysicsComponent::PostInit(void) {

}
/** Updates each of the components attributes
 ** time: current game time
**/
void PhysicsComponent::update(float time) {
	
}

/** Renders the component
 ** window: canvas on which to draw the component
 ** 
**/
void PhysicsComponent::render(sf::RenderWindow *window) {

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
