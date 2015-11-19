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
ComponentId CollectorComponent::getId(void) const {
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

/** Destructor
 **
 **/
CollectorComponent::~CollectorComponent() {
	vase_sprites.clear();
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
		vases_num = vases;				
                if (*temp != '\0') {
                    std::cout << "CollectorComponent::Init: Failed to initialize: Error reading attribute for " << attr.name() << std::endl;
                    return false;
                }					
            }
		else  if (!strcmp(attr.name(), "Size")) {
                vase_size = std::strtol(attr.value(), &temp, 10);				
                if (*temp != '\0') {
                    std::cout << "CollectorComponent::Init: Failed to initialize: Error reading attribute for " << attr.name() << std::endl;
                    return false;
                }					
            }
		else if (!strcmp(attr.name(), "FireFlower")) {
			vase_fireflower.loadFromFile(("./assets/sprites/" + (std::string)attr.value()).c_str());
		}
		else if (!strcmp(attr.name(), "EarthFlower")) {
			vase_earthflower.loadFromFile(("./assets/sprites/" + (std::string)attr.value()).c_str());
		}
		else if (!strcmp(attr.name(), "WaterFlower")) {
			vase_waterflower.loadFromFile(("./assets/sprites/" + (std::string)attr.value()).c_str());
		}
		else if (!strcmp(attr.name(), "AirFlower")) {
			vase_airflower.loadFromFile(("./assets/sprites/" + (std::string)attr.value()).c_str());
		}
		else if (!strcmp(attr.name(), "Empty")) {
			vase_empty.loadFromFile(("./assets/sprites/" + (std::string)attr.value()).c_str());
		}
        }
    }
	
    return true;
}

/** Final Initilizer
 ** Setups up additional attributes based on game configuration
 **/
bool CollectorComponent::PostInit(pugi::xml_node* elem) {
	if (elem != NULL)
	Init(elem);
	if (owner->isOfType("Player")) {
		for (int i = 0; i < vases; i++) {
			vase_sprites.push_back(sf::Sprite(vase_empty, sf::IntRect(0, 0, vase_empty.getSize().x, vase_empty.getSize().y)));
			vase_sprites.back().scale(1.0*vase_size/(vase_empty.getSize().x), 1.0*vase_size/(vase_empty.getSize().y));
			vase_sprites.back().setPosition(0,0);
		}
	}
	owner->addDelegate(CollectEvent::event_type);
	return true;
}

bool CollectorComponent::PostInit(void) {

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
	StrongActorPtr other_actor =  EventManagerInterface::getActor(e->getSender());

	if (other_actor == NULL)
		return;

	if (event_type == ContactEvent::event_type) {
		if (other_actor->hasComponent(CollectableComponent::id)) {
			if (flowers < vases ) {
				if (!EventManagerInterface::get()->queueEvent(new CollectEvent(e->getTimeStamp(), owner->getInstance(), other_actor->getInstance())))
					std::cout << "CollectableComponent::update: Unable to queue event" << std::endl;		
			}		
		}
		else if (owner->isOfType("Player")) {
			if (vases > 0 && other_actor->causesDamage()) {
				if (vases == flowers) {
					flowers--;
					flowerList.erase(flowerList.end());
				}
				vases--;
				vase_sprites.erase(vase_sprites.end());
			}
		}
	}
	else if (event_type == CollectEvent::event_type) {	
		LevelView::removeActor(other_actor->getInstance());		
		if (owner->isOfType("Player")) {				
			flowerList.push_back(other_actor);
			if (other_actor->isOfType("FireFlower")) {
				vase_sprites[flowers++].setTexture(vase_fireflower);
			}
			else if (other_actor->isOfType("EarthFlower")) {
				vase_sprites[flowers++].setTexture(vase_earthflower);
			}
			else if (other_actor->isOfType("AirFlower")) {
				vase_sprites[flowers++].setTexture(vase_airflower);
			}
			else if (other_actor->isOfType("WaterFlower")) {
				vase_sprites[flowers++].setTexture(vase_waterflower);
			}
		}
	}	
}

/** Reset the component
 **
 **/
    void CollectorComponent::reset(void) {
	vases = vases_num;
	flowers = 0;
	flowerList.clear();
	vase_sprites.clear();
	for (int i = 0; i < vases; i++) {
			vase_sprites.push_back(sf::Sprite(vase_empty, sf::IntRect(0, 0, vase_empty.getSize().x, vase_empty.getSize().y)));
	}
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
                std::vector<StrongActorPtr> CollectorComponent::getFlowers(void){
                    return flowerList;
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

/** Renders component
 ** window: current game render window
 **/
void CollectorComponent::render(sf::RenderWindow *window, bool minimap) {
	if (owner->isOfType("Player") && !minimap) {
		std::vector<sf::Sprite>::iterator it;
		int i = 0;
		int sep = 2;

		sf::Vector2f gameView_top_corner = Configuration::getGameViewCenter();
		gameView_top_corner.x -= Configuration::getGameViewWidth()/2;
		gameView_top_corner.y -= Configuration::getGameViewHeight()/2;
	
		for (it = vase_sprites.begin(); it != vase_sprites.end(); ++it) {
			if (*(owner->getGameState()) == 0) {
				(*it).setScale(5.0*vase_size/(vase_empty.getSize().x), 5.0*vase_size/(vase_empty.getSize().y));
				(*it).setPosition( gameView_top_corner.x + (i++)*(5* vase_size +sep), gameView_top_corner.y);

			}
			else {
				(*it).setScale(1.0*vase_size/(vase_empty.getSize().x), 1.0*vase_size/(vase_empty.getSize().y));
				(*it).setPosition( gameView_top_corner.x + (i++)*(vase_size +sep), gameView_top_corner.y);

			}						
			window->draw(*it);
		}
	}
	
}
