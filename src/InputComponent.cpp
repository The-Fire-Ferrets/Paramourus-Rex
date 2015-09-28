#include "InputComponent.h"

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
InputComponent::InputComponent(void) {
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
			if (!strcmp(attr.name(),"Type")) {
				type = attr.value();
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
	if (type == "Artificial") {
		//Perform AI Stuff
	}
	else if (type == "Keyboard") {
		//Reads Input and perform actions
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
