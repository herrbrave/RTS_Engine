#ifndef __EVENT_H__
#define __EVENT_H__

#include"Constants.h"
#include"SDL_Helpers.h"
#include"vector2f.h"

#include<list>

static std::atomic_ulong sId{ 1 };

class EventData;

using std::list;
using std::function;

enum class EventType : Uint8 {
	ENTITY_CREATED = 0,
	ENTITY_POSITION_SET = 1,
	ENTITY_COLLISION_SET = 2,
	MAP_LOADED = 3,
	ENTITY_ZORDER_SET = 4,
	ENTITY_DESTROYED = 99
};

typedef shared_ptr<EventData> EventDataPtr;
typedef function<void(const EventData&)> EventDelegate;

struct EventListenerDelegate {
	EventDelegate invokable;
	unsigned long id;

	EventListenerDelegate(const EventDelegate& eventDelegate) {
		id = sId.fetch_add(1);
		invokable = eventDelegate;
	}

	void operator()(const EventData& eventData) {
		invokable(eventData);
	}
};

typedef list<EventListenerDelegate> EventDelegateList;
typedef unordered_map<EventType, EventDelegateList> EventDataListeners;
typedef list<EventDataPtr> EventDataQueue;

class EventData {
public:
	EventData(Uint32 timestamp) {
		mTimestamp = timestamp;
	}

	virtual const EventType getEventType() = 0;
	const Uint32 getTimestamp() { return mTimestamp;  }

private:
	Uint32 mTimestamp;
};

class EntityCreatedEventData : public EventData {
public:
	EntityCreatedEventData(unsigned long entityId, Uint32 timestamp) : EventData(timestamp) {
		mEntityId = entityId;
	}

	const EventType getEventType() override {
		return EventType::ENTITY_CREATED;
	}

	const unsigned long getEntityId() {
		return mEntityId;
	}

private:
	unsigned long mEntityId;
};

class EntityPositionSetEventData : public EventData {
public:
	EntityPositionSetEventData(unsigned long entityId, const Vector2f& position, Uint32 timestamp) : EventData(timestamp) {
		mEntityId = entityId;
		mPosition = Vector2fPtr(GCC_NEW Vector2f(position));
	}

	const EventType getEventType() override {
		return EventType::ENTITY_POSITION_SET;
	}

	const unsigned long getEntityId() {
		return mEntityId;
	}

	const Vector2f& getPosition() {
		return mPosition;
	}

private:
	unsigned long mEntityId;
	Vector2fPtr mPosition;
};

class EntityCollisionSetEventData : public EventData {
public:
	EntityCollisionSetEventData(unsigned long entityId, Uint32 timestamp) : EventData(timestamp) {
		mEntityId = entityId;
	}

	const EventType getEventType() override {
		return EventType::ENTITY_COLLISION_SET;
	}

	const unsigned long getEntityId() {
		return mEntityId;
	}

private:
	unsigned long mEntityId;
};

class EntityZOrderSetEventData : public EventData {
public:
	EntityZOrderSetEventData(unsigned long entityId, Uint32 timestamp) : EventData(timestamp) {
		mEntityId = entityId;
	}

	const EventType getEventType() override {
		return EventType::ENTITY_COLLISION_SET;
	}

	const unsigned long getEntityId() {
		return mEntityId;
	}

private:
	unsigned long mEntityId;
};

class MapLoadedSetEventData : public EventData {
public:
	MapLoadedSetEventData(Uint32 timestamp) : EventData(timestamp) {}

	const EventType getEventType() override {
		return EventType::MAP_LOADED;
	}
};

class EntityDestroyedEventData : public EventData {
public:
	EntityDestroyedEventData(unsigned long entityId, Uint32 timestamp) : EventData(timestamp) {
		mEntityId = entityId;
	}

	const EventType getEventType() override {
		return EventType::ENTITY_DESTROYED;
	}

	const unsigned long getEntityId() {
		return mEntityId;
	}

private:
	unsigned long mEntityId;
};

class EventManager {
public:
	EventManager(EventManager const& eventManager) = delete;
	void operator=(EventManager const&) = delete;

	static EventManager& getInstance() {
		static EventManager instance;
		return instance;
	}

	void addDelegate(const EventListenerDelegate& eventDelegate, EventType eventType) {
		EventDelegateList& eventDelegates = mEventListeners.at(eventType);
		eventDelegates.push_back(eventDelegate);
	}

	void removeDelegate(const EventListenerDelegate& eventDelegate, EventType eventType) {
		EventDelegateList& eventDelegates = mEventListeners.at(eventType);

		for (auto it = eventDelegates.begin(); it != eventDelegates.end(); ++it) {
			if (it->id == eventDelegate.id) {
				eventDelegates.erase(it);
				break;
			}
		}
	}

	void update() {
		while (mQueue.size() > 0) {
			EventDataPtr eventData = mQueue.front();
			mQueue.pop_front();

			EventDelegateList eventDelegates = mEventListeners.at(eventData->getEventType());
			for (auto eventDelegate : eventDelegates) {
				eventDelegate(*eventData);
			}
		}
	}

	void pushEvent(EventData* eventData) {
		mQueue.push_back(EventDataPtr(eventData));
	}

private:
	EventDataQueue mQueue;
	EventDataListeners mEventListeners;

	EventManager() {
		mEventListeners.emplace(EventType::ENTITY_CREATED, EventDelegateList());
		mEventListeners.emplace(EventType::ENTITY_COLLISION_SET, EventDelegateList());
		mEventListeners.emplace(EventType::ENTITY_POSITION_SET, EventDelegateList());
		mEventListeners.emplace(EventType::MAP_LOADED, EventDelegateList());
		mEventListeners.emplace(EventType::ENTITY_DESTROYED, EventDelegateList());
		mEventListeners.emplace(EventType::ENTITY_ZORDER_SET, EventDelegateList());
	}
};

#endif // !__EVENT_H__
