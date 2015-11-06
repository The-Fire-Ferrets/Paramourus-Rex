#include "CraftableComponent.h"
#include "CraftEvent.h"
#include "CraftView.h"

//Unique instance id among instances of the same component
int CraftableComponent::instances = -1;
//Unique component id
ComponentId CraftableComponent::id = "CraftableComponent";
//Registers component with factory
const bool CraftableComponent::registered = ActorFactory::registerComponent(&id, &create);

/** Returns the id shared by all components of this type
 **
 **/
ComponentId CraftableComponent::getId(void) const {
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

/** Destructor
 **
 **/
CraftableComponent::~CraftableComponent() {
	elements.clear();
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
				elements.push_back(type);
            }
        }
    }
    return true;
}

/** Renders component
 ** window: current game render window
 **/
void CraftableComponent::render(sf::RenderWindow *window, bool minimap) {
	
}
/** Final Initilizer
 ** Setups up additional attributes based on game configuration
 **/
bool CraftableComponent::PostInit(pugi::xml_node* elem) {
	if (elem != NULL)
	Init(elem);
    owner->addDelegate(CraftEvent::event_type);
	return true;
}

/** Updates the component's attributes
 ** time: current game time
 **/
void CraftableComponent::update(float time) {

}

/** Receives event when the actor is being contacted by another actor and responds by accordingly
 **
 **/
void CraftableComponent::update(EventInterfacePtr e) {
    EventType event_type = e->getEventType();
    StrongActorPtr sender = CraftView::getFlower(e->getSender());
	StrongActorPtr receiver = CraftView::getFlower(e->getReceiver());


	// item crafted
    if (event_type == CraftEvent::event_type && this->owner == receiver) {
		// get a pointer to the sender's colletable component
		std::shared_ptr<ActorComponent> sender_ac, receiver_ac;
		std::shared_ptr<CraftableComponent> sender_cc, receiver_cc;

		sender_ac   = sender->components[CraftableComponent::id];
		receiver_ac = receiver->components[CraftableComponent::id];
		sender_cc = std::dynamic_pointer_cast<CraftableComponent>(sender_ac);
		receiver_cc = std::dynamic_pointer_cast<CraftableComponent>(receiver_ac);

		if (receiver_cc->doesCombineWith(*sender_cc)) {
			// do craft
			receiver_cc->combineWith(*sender_cc);

			// sender no longer exists
			CraftView::removeFlower(sender);
			sender.reset();

			// let the CraftView the event is finished
			if (!EventManagerInterface::get()->queueEvent(new CraftEvent(0.f, receiver->getInstance(), -1))) {
				std::cout << "CraftableComponent::update: unable to send response to CraftView" << std::endl;
			}
		}

	}
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

/** Checks whether a combination with the given CraftableComponent
 ** is possible.
 **/
bool CraftableComponent::doesCombineWith(const CraftableComponent& other) const {
	// get resulting component list
	std::vector<std::string> combo;
	combo.insert(combo.end(), elements.begin(), elements.end());
	combo.insert(combo.end(), other.elements.begin(), other.elements.end());

	// open the recipe book
	pugi::xml_document doc;
	pugi::xml_parse_result result = doc.load_file("./assets/Recipes.xml");
	if (!result) {
		std::cout << "CraftableComponent: unable to load recipe book" << std::endl;
		return false;
	}

	// look at each result
    pugi::xml_node recipes = doc.child("Recipes");
    for (pugi::xml_node recipe = recipes.first_child(); recipe; recipe = recipe.next_sibling()) {
		for (pugi::xml_attribute attr = recipe.first_attribute(); attr; attr = attr.next_attribute()) {
			if (!strcmp(attr.name(), "Components")) {
				// are the component lists equivalent?
				std::vector<std::string> components = split(std::string(attr.value()), ' ');
				if (have_equivalent_strings(combo, components)) return true;
			}
		}
	}

	return false;
}

/** Incorporate the components of another CraftableComponent into
 ** this one.
 **/
void CraftableComponent::combineWith(const CraftableComponent& other) {
	// sanity check
	if (!this->doesCombineWith(other)) {
		return;
	}

	// get resulting component list
	elements.insert(elements.end(), other.elements.begin(), other.elements.end());

	// open the recipe book
	pugi::xml_document doc;
	pugi::xml_parse_result result = doc.load_file("./assets/Recipes.xml");
	if (!result) {
		std::cout << "CraftableComponent: unable to load recipe book" << std::endl;
		return;
	}

	// look at each result
    pugi::xml_node recipes = doc.child("Recipes");
    for (pugi::xml_node recipe = recipes.first_child(); recipe; recipe = recipe.next_sibling()) {
		for (pugi::xml_attribute attr = recipe.first_attribute(); attr; attr = attr.next_attribute()) {
			// find matching components
			if (!strcmp(attr.name(), "Components")) {
				std::vector<std::string> components = split(std::string(attr.value()), ' ');
				if (have_equivalent_strings(elements, components)) {
					std::string type = this->getCraftResultValue(recipe, "Type");
					std::string sprite = this->getCraftResultValue(recipe, "Value");

					// change the owner actor to the appropirate type
					this->type = type;
					owner->sprite_texture[0].loadFromFile(sprite);
					owner->sprite[0].setTexture(owner->sprite_texture[0]);
				}
			}
		}
	}
}

std::string CraftableComponent::getCraftResultValue(pugi::xml_node node, std::string name) {
	for (pugi::xml_attribute attr = node.first_attribute(); attr; attr = attr.next_attribute()) {
		// find matching components
		if (!strcmp(attr.name(), name.c_str())) {
			return std::string(attr.value());
		}
	}
}
