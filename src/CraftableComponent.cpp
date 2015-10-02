#include "CraftableComponent.h"

//Unique instance id among instances of the same component
int CraftableComponent::instances = -1;
//Unique component id
ComponentId CraftableComponent::id = "CraftableComponent";
//Registers component with factory
const bool CraftableComponent::registered = ActorFactory::registerComponent(&id, &create);

/** Returns the id shared by all components of this type
 **
**/
ComponentId CraftableComponent::getId(void) {
	return id;
}

/** Returns a reference to the components constructor
 **
**/
ActorComponent* CraftableComponent::create() { 
	//update the instance count
	instances++;
	return new CraftableComponent();
}

/** Constructor
 ** Sets up unique instance ID
**/
CraftableComponent::CraftableComponent(void) {
	instance = instances;
}

/** Initializer
 ** elem : node pointing to section of XML configuration holding more attribute defaults to setup
 ** Sets up additional attribute defaults
**/
bool CraftableComponent::Init(pugi::xml_node* elem) {	
	//Iterate over the component's attributes
	for (pugi::xml_node tool = elem->first_child(); tool; tool = tool.next_sibling()) {
		for (pugi::xml_attribute attr = tool.first_attribute(); attr; attr = attr.next_attribute()) {
			if (!strcmp(attr.name(), "Type")) {
				type = attr.value();								
			}
		}
	}
	return true;
}

/** Final Initilizer
 ** Setups up additional attributes based on game configuration
**/
void CraftableComponent::PostInit(void) {
}

/** Updates the component's attributes
 ** time: current game time
**/
void CraftableComponent::update(float time) {
	
}


/** Reset the component
 **
**/
void CraftableComponent::reset(void) {

}

/** Restart the component
 **
**/
void CraftableComponent::restart(void) {

}

/** Cleans up after quiting
 **
**/
void CraftableComponent::quit(void) {

}
