#include "CollectorComponent.h"

//Unique instance id among instances of the same component
int CollectorComponent::instances = -1;
//Unique component id
ComponentId CollectorComponent::id = "CollectorComponent";
//Registers component with factory
const bool CollectorComponent::registered = ActorFactory::registerComponent(&id, &create);

/** Returns the id shared by all components of this type
 **
**/
ComponentId CollectorComponent::getId(void) {
	return id;
}

/** Returns a reference to the components constructor
 **
**/
ActorComponent* CollectorComponent::create() { 
	//update the instance count
	instances++;
	return new CollectorComponent();
}

/** Constructor
 ** Sets up unique instance ID
**/
CollectorComponent::CollectorComponent(void) {
	instance = instances;
	flowers = 0;
}

/** Initializer
 ** elem : node pointing to section of XML configuration holding more attribute defaults to setup
 ** Sets up additional attribute defaults
**/
bool CollectorComponent::Init(pugi::xml_node* elem) {	
	char * temp;
	//Iterate over the component's attributes
	for (pugi::xml_node tool = elem->first_child(); tool; tool = tool.next_sibling()) {
		for (pugi::xml_attribute attr = tool.first_attribute(); attr; attr = attr.next_attribute()) {
			if (!strcmp(attr.name(), "Vases")) {
				vases = std::strtol(attr.value(), &temp, 10);				
				if (*temp != '\0') {
					std::cout << "CollectorComponent::Init: Failed to initialize: Error reading attribute for " << attr.name() << std::endl;
					return false;
				}					
			}
		}
	}
	return true;
}

/** Final Initilizer
 ** Setups up additional attributes based on game configuration
**/
void CollectorComponent::PostInit(void) {
}

/** Updates the component's attributes
 ** time: current game time
**/
void CollectorComponent::update(float time) {
	
}

/** Receives event when the actor has collected something
 ** Adds flower to cllection
 ** Updates it cases
**/
void CollectorComponent::update(EventInterfacePtr e) {
	EventType event_type = e->getEventType();
	StrongActorPtr other_actor = LevelView::getActor(e->getSender());
	if (event_type == ContactEvent::event_type) {
		if (other_actor->hasComponent(CollectableComponent::id)) {
			if (vases > 0) {
				if (!EventManagerInterface::get()->queueEvent(new CollectEvent(e->getTimeStamp(), owner->getInstance(), other_actor->getInstance())))
					std::cout << "CollectableComponent::update: Unable to queue event" << std::endl;		
				setVases(getVases()-1);
				flowerList.push_back(other_actor);
				flowers++;
				std::cout << owner->getId() << "  collecting " << other_actor->getId() << " vase number now " << vases << std::endl;			}		
		}
		else if (owner->getId() == "Player") {
			setVases(getVases()-1);
			std::cout << owner->getId() << "  lost a vase and has " << vases << std::endl;
		}
	}	
}

/** Reset the component
 **
**/
void CollectorComponent::reset(void) {

}

/** Restart the component
 **
**/
void CollectorComponent::restart(void) {

}

/** Cleans up after quiting
 **
**/
void CollectorComponent::quit(void) {

}

/** Returns the list of collected flowers
**
**/
StrongActorPtr *CollectorComponent::getFlowers(void){
	return *flower_list;
}

/** Sets the numnber of vases
 ** b: new number of vases
**/
void CollectorComponent::setVases(int v) {
  if (v >= 0) {
	  vases = v;
  }
}

/** Returns the number of vases
 **
**/
int CollectorComponent::getVases(void) {
	return vases;
}
