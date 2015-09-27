#include "CollectableComponent.h"

//Unique instance id among instances of the same component
int CollectableComponent::instances = -1;
//Unique component id
ComponentId CollectableComponent::id = "CollectableComponent";
//Registers component with factory
const bool CollectableComponent::registered = ActorFactory::registerComponent(&id, &create);

/** Returns the id shared by all components of this type
 **
**/
ComponentId CollectableComponent::getId(void) {
	return id;
}

/** Returns a reference to the components constructor
 **
**/
ActorComponent* CollectableComponent::create() { 
	//update the instance count
	instances++;
	return new CollectableComponent();
}

/** Constructor
 ** Sets up unique instance ID
**/
CollectableComponent::CollectableComponent(void) {
	instance = instances;
}

/** Initializer
 ** elem : node pointing to section of XML configuration holding more attribute defaults to setup
 ** Sets up additional attribute defaults
**/
bool CollectableComponent::Init(pugi::xml_node* elem) {
	if (debug_mode)
		std::cout << elem->name() << std::endl;
	
	char * temp;
	//Iterate over the component's attributes
	for (pugi::xml_node tool = elem->first_child(); tool; tool = tool.next_sibling()) {
		for (pugi::xml_attribute attr = tool.first_attribute(); attr; attr = attr.next_attribute()) {
			if (!strcmp(attr.name(), "Bags")) {
				bags = std::stoi(attr.value(), temp);				
				if (*temp != '\0') {
					std::cout << "CollectableComponent::Init: Failed to initialize: Error reading attribute for " << attr.name() << std::endl;
					return false
				}					
			}
			if (debug_mode)
				std::cout << attr.value() << std::endl;
		}
	}
	return true;
}

/** Final Initilizer
 ** Setups up additional attributes based on game configuration
**/
void CollectableComponent::PostInit(void) {
}

/** Updates the component's attributes
 ** time: current game time
**/
void CollectableComponent::update(float time) {
	
}

/** Renders the component
 ** window: canvas on which to draw the component
 ** 
**/
void CollectableComponent::render(sf::RenderWindow *window) {
}

/** Reset the component
 **
**/
void CollectableComponent::reset(void) {

}

/** Restart the component
 **
**/
void CollectableComponent::restart(void) {

}

/** Cleans up after quiting
 **
**/
void CollectableComponent::quit(void) {

}

void CollectableComponent::setBags(int b) {
	bags = b;
