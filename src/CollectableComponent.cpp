#include "CollectableComponent.h"
#include "CraftEvent.h"

//Unique instance id among instances of the same component
int CollectableComponent::instances = -1;
//Unique component id
ComponentId CollectableComponent::id = "CollectableComponent";
//Registers component with factory
const bool CollectableComponent::registered = ActorFactory::registerComponent(&id, &create);

/** Returns the id shared by all components of this type
 **
 **/
ComponentId CollectableComponent::getId(void) const {
    return id;
}

/** Returns a reference to the components constructor
 **
 **/
ActorComponent* CollectableComponent::create() { 
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

/** Destructor
 ** updates the reference hount on the collector
 **/
CollectableComponent::~CollectableComponent() {
	// deletes when ref count is 0
	collector.reset();
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
bool CollectableComponent::PostInit(pugi::xml_node* elem) {
	if (elem != NULL)
	Init(elem);
    owner->addDelegate(CollectEvent::event_type);
	return true;
}

bool CollectableComponent::PostInit(void) {
	
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
    StrongActorPtr other_actor =  EventManagerInterface::getActor(e->getSender());
	if (other_actor == NULL)
		return;

	// item collection
    if (event_type == CollectEvent::event_type && owner->getVisible()) {     
	owner->setVisible(false);
        collector = other_actor;
	Pathfinder::removeFromPaths(owner->getInitialPosition());
        owner->setPosition(sf::Vector2f(-1000, 0));
	LevelView::flowers_left--;
	if (!EventManagerInterface::get()->queueEvent(new CollectEvent(e->getTimeStamp(), owner->getInstance(), other_actor->getInstance())))
		std::cout << "CollectableComponent::update: Unable to queue event" << std::endl;
	//std::cout << owner->getId() << " " << owner->getInstance() << "  collected by " << other_actor->getId() << " " << other_actor->getInstance() << std::endl;
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

/** Renders component
 ** window: current game render window
 **/
void CollectableComponent::render(sf::RenderWindow *window, bool minimap) {
	
}

/** Cleans up after quiting
 **
 **/
void CollectableComponent::quit(void) {

}
