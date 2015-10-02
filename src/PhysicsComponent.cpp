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

PhysicsComponent::~PhysicsComponent(void) {	

}

/** Initilizer
 ** elem : node pointing to section of XML configuration holding more attribute defaults to setup
 ** Sets up additional attribute defaults
**/
bool PhysicsComponent::Init(pugi::xml_node* elem) {;
	for (pugi::xml_node tool = elem->first_child(); tool; tool = tool.next_sibling()) {
		for (pugi::xml_attribute attr = tool.first_attribute(); attr; attr = attr.next_attribute()) {
		}
	}
	
	return true;
	
}

/** Final Initilizer
 ** Setups up additional attributes based on game configuration
**/
void PhysicsComponent::PostInit(void) {

}

/** Checks to see if the bound of two actor intersect and sends ContactEvent to the event manager
 ** time: current game time
**/
void PhysicsComponent::update(float time) {
	for (int i = 0; i < LevelView::getNumActors(); i ++) {
		StrongActorPtr other_actor = LevelView::actors[i];
		if (owner->getInstance() != other_actor->getInstance()) {
			if ((owner->getBoundary())->intersects(*(other_actor->getBoundary()))) {
				if (!EventManagerInterface::get()->queueEvent(new ContactEvent(time, owner->getInstance(), other_actor->getInstance()))) {
					std::cout << "PhysicsComponent::update: Unable to queue event" << std::endl;
				}		
			}
		}
	}
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
	this->~PhysicsComponent();
}
