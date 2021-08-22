#include"Entity.h"

Entity::Entity(const rapidjson::Value& root) {
	/*if (root.FindMember("children") != root.MemberEnd()) {
		EntityPtr entity = std::make_shared<Entity>(this);
		for (auto it = root["children"].Begin(); it != root["children"].End(); it++) {
			auto child = std::make_shared<Entity>(*it);
			child->parent = entity;
			children.push_back(child);
		}
	}*/

	for (auto it = root["components"].MemberBegin(); it != root["components"].MemberEnd(); it++) {
		const rapidjson::Value& component = it->value;

		switch (COMPONENT_VALUE_MAP.at((Uint8) std::stoi(it->name.GetString()))) {
			case ComponentType::DRAWABLE_COMPONENT: {
				this->addComponent(std::make_shared<DrawableComponent>(this->id, component));
				break;
			}
			case ComponentType::ANIMATION_COMPONENT: {
				this->addComponent(std::make_shared<AnimationComponent>(this->id, component));
				break;
			}
		}
	}
}

void Entity::update() {
	if (mStateQueue.size() <= 0) {
		return;
	}

	auto currentState = mStateQueue[0];
	if (!currentState->hasBegun()) {
		currentState->begin();
	}

	if (currentState->isEnded()) {
		mStateQueue.erase(std::begin(mStateQueue));
		if (mStateQueue.size() > 0) {
			currentState = mStateQueue[0];
			currentState->begin();
		}
		else {
			currentState = nullptr;
		}
	}

	if (currentState != nullptr) {
		currentState->update();
	}
}

void Entity::pushState(StatePtr state) {
	mStateQueue.push_back(state);
}

void Entity::addComponent(ComponentPtr component) {
	mComponents.emplace(component->componentId, component);
}

Components& Entity::getComponents() {
	return mComponents;
}

void  Entity::serialize(Serializer& serializer) const {
	serializer.writer.StartObject();

	serializer.writer.String("children");
	serializer.writer.StartArray();
	for (auto child : children) {
		child->serialize(serializer);
	}
	serializer.writer.EndArray();

	serializer.writer.String("components");
	serializer.writer.StartObject();
	for (auto component : mComponents) {
		serializer.writer.String(std::to_string(static_cast<std::underlying_type<ComponentType>::type>(component.first)).c_str());
		component.second->serialize(serializer);
	}
	serializer.writer.EndObject();

	serializer.writer.EndObject();
}