#include "ActorFactory.h"

//Total size of pointer arrays
const int ActorFactory::size = 20;
//Array holding the registered components
ComponentId* ActorFactory::actorComponentIds[size];
//Array holding reference to actor component creators
ActorComponentCreator ActorFactory::actorComponentCreators[size];
//List holding component creators
std::vector<ComponentId> ActorFactory::componentCreatorList;
/** Creates an actor and all its components based on XML configuration
 ** resource: filename for xml
 ** state: current game state
 **/
StrongActorPtr ActorFactory::CreateActor(const char* resource, int* state) {
    //Holds referenced to loaded XML file	
    pugi::xml_document doc;

    //Error check to see if file was loaded corectly
    pugi::xml_parse_result result;
    std::string resource_str(resource);
    if (!(result = doc.load_file(("./assets/actors/" + resource_str + ".xml").c_str()))) {
        std::cout << "ActorFactory::CreateActor(...): Failed to load" << std::endl;
        std::cout << "Filename: " << resource << " Load result: " << result.description() << std::endl;
        return StrongActorPtr();
    }

    // Constructs a default actor and gives in the current game state
    StrongActorPtr actor(new Actor());
    actor->resetGameState(state);

    //Used to iterate over XML file to get attributes
    pugi::xml_node tools = doc.child(resource);

    //Initializes the specified actor with chosen XML attributes
    if(!actor->Init(&tools)) {
        std::cout << "ActorFactory::CreateActor(...): Failed to initialize actor: " << resource << std::endl;
        return StrongActorPtr();
    }

    //Iterates over XML to get components to add
    for (pugi::xml_node tool = tools.first_child(); tool; tool = tool.next_sibling()) {
        //Creates each component given the XML attribute
        StrongActorComponentPtr component(CreateComponent(&tool));
        if (component) {
            //Adds the component to the actor
            actor->AddComponent(component);
            //Gives the component a reference to its owner
            component->SetOwner(actor);
            //Completes any post-initialization of the components
            component->PostInit();
        }
        else {
            std::cout << "ActorFactory::CreateActor(...): Failed to create component" << std::endl;
            return StrongActorPtr();
        }
    }

    return actor;
}

/** Creates actor components
 ** elem: reference to section in XML to continue reading from
 **/
StrongActorComponentPtr ActorFactory::CreateComponent(pugi::xml_node* elem) {
    //Reference to the component id
    ComponentId componentid = elem->name();
    //Pointer to created component
    StrongActorComponentPtr component;

    //Find the index of the component id if it is registered (using a map would be better but couldn't find a way to implement a growing map statically)
    int idx = -1;
    for (int i = 0; i < size; i++) {	
        if (componentid == *actorComponentIds[i]) {
            idx = i;
            break;
        }
    }
    //Map the component creator using the idx if found  (using a map would be better but couldn't find a way to implement a growing map statically)
    if (idx >= 0) {	
        //Calls component creator and sets the component pointer to the created component reference
        ActorComponentCreator creator = actorComponentCreators[idx];
        component.reset(creator());
    }
    else {
        std::cout << "ActorFactory::CreateComponent(...): Component: " << component << " is not registered" << std::endl;
        return StrongActorComponentPtr();
    }

    if (component) {
        if (!component->Init(elem)) {
            return StrongActorComponentPtr();
        }
    }
    else {
        std::cout << "ActorFactory::CreateComponent(...): Failed to create component: " << componentid << std::endl;
        return StrongActorComponentPtr();
    }

    return component;

}

/** Used during compile time to register each component and ID with the factor to be used during creation
 ** id: unique string referring to each component
 ** creator: used to call the appropriate constructor
 **/
    bool ActorFactory::registerComponent(ComponentId* id, ActorComponentCreator creator) {
        //Finds the next empty location with which to register the component (using a map would be better but couldn't find a way to implement a growing map statically)
        int idx = -1;
        for (int i = 0; i < size; i++) {
            if (!(ActorFactory::actorComponentIds[i])) {
                idx = i;
                break;
            }
        }

        //Adds references to the component id and its creator to arrays  (using a map would be better but couldn't find a way to implement a growing map statically)
        //std::cout << componentCreatorList.size() << std::endl;
        componentCreatorList.push_back(*id);
        //std::cout << componentCreatorList.size() << std::endl;
        if (idx >= 0) {
            ActorFactory::actorComponentCreators[idx] = creator;
            ActorFactory::actorComponentIds[idx] = id;
            return true;
        }
        else {
            std::cout << "ActorFactory::registerComponent(...): Failed to register component: " << id << std::endl;
            return false;
        }
    }

void ActorFactory::reset(void) {
    //Actor::instances = 1;
}
