#include "ActorComponent.h"

/** Changes the owner of the component
 ** p_owner: pointer to component owner
 **/
void ActorComponent::SetOwner(StrongActorPtr p_owner) {
    owner = p_owner;

}

/** returns the components instance
 **
 **/
int ActorComponent::getInstance(void) {
    return instance;
}

/** returns the component's type
 **
 **/
std::string ActorComponent::getType(void) {
    return type;
}
