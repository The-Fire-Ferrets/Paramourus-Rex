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

/** test for equality; the components must be of the same type
 ** and be of the same instance of that type
 **/
bool ActorComponent::operator==(const ActorComponent& rhs) const {
	return (this->instance == rhs.instance) && (this->getId() == rhs.getId());
}
