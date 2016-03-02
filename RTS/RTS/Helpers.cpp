#include"Helpers.h"

const p_vector2f& getEntityPosition(Entity* entity, EntitySystem* entitySystem) {
	PhysicsComponent* physicsComponent = reinterpret_cast<PhysicsComponent*>(entity->componentContainer->getComponentByType(ComponentType::PHYSICS_COMPONENT));
	p_vector2f position(physicsComponent->getPosition);

	unsigned long parentId = entity->id;
	while (parentId != -1) {
		Entity* parent = entitySystem->getEntityById(parentId);
		physicsComponent = reinterpret_cast<PhysicsComponent*>(parent->componentContainer->getComponentByType(ComponentType::PHYSICS_COMPONENT));
		*position.get() += *physicsComponent->getPosition();
		parentId = parent->id;
	}

	return position;
}