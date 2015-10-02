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
	collector = NULL;
}

/** Initializer
 ** elem : node pointing to section of XML configuration holding more attribute defaults to setup
 ** Sets up additional attribute defaults
**/
bool CollectableComponent::Init(pugi::xml_node* elem) {	
	//Iterate over the component's attributes
	for (pugi::xml_node tool = elem->first_child(); tool; tool = tool.next_sibling()) {
		for (pugi::xml_attribute attr = tool.first_attribute(); attr; attr = attr.next_attribute()) {
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

/** Receives event when the actor is being contacted by another actor
 ** If contacting actor has a collectabecomponent, send actor contactevent to ackowledge collection
 ** Removes itself from the board
**/
void CollectableComponent::update(EventInterfacePtr e) {
	EventType event_type = e->getEventType();
	StrongActorPtr other_actor = LevelView::actors[e->getSender()];
	if (event_type == ContactEvent::event_type) {
		if (other_actor->hasComponent(CollectorComponent::id)) {
			std::cout << owner->getId() << "  collected by " << other_actor->getId() << std::endl;
			if (!EventManagerInterface::get()->queueEvent(new ContactEvent(e->getTimeStamp(), owner->getInstance(), other_actor->getInstance()))) {
				std::cout << "CollectableComponent::update: Unable to queue event" << std::endl;
			}
			owner->setVisible(false);
			collector = other_actor;
			owner->setPosition(sf::Vector2f(-1000, 0));
		}
	}	
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
