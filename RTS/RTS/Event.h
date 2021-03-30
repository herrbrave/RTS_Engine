#ifndef __EVENT_H__
#define __EVENT_H__

#include"Constants.h"
#include"Input.h"
#include"SDL_Helpers.h"
#include"vector2f.h"
#include"Script.h"

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
	ENTITY_COLLISION_EVENT = 5,
	LOAD_ASSET = 6,
	MOUSE_EVENT = 7,
	KEY_EVENT = 8,
	SCRIPT_LOADED = 9,	
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
		return EventType::ENTITY_ZORDER_SET;
	}

	const unsigned long getEntityId() {
		return mEntityId;
	}

private:
	unsigned long mEntityId;
};

class EntityCollisionEventData : public EventData {
public:
	EntityCollisionEventData(unsigned long colliderEntityId, unsigned long collidedEntityId, Uint32 timestamp) : EventData(timestamp) {
		this->mColliderEntityId = colliderEntityId;
		this->mCollidedEntityId = collidedEntityId;
	}

	const EventType getEventType() override {
		return EventType::ENTITY_COLLISION_EVENT;
	}

	const unsigned long getColliderEntityId() {
		return mColliderEntityId;
	}

	const unsigned long getCollidedEntityId() {
		return mCollidedEntityId;
	}

private:
	unsigned long mColliderEntityId;
	unsigned long mCollidedEntityId;
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


class LoadAssetEventData : public EventData {
public:
	string path;
	string assetTag;

	LoadAssetEventData(Uint32 timestamp, string& path, string& assetTag) : EventData(timestamp) {
		this->path = path;
		this->assetTag = assetTag;
	}

	const EventType getEventType() override {
		return EventType::LOAD_ASSET;
	}
};

class MouseEventData : public EventData {
public:
	float x, y;
	MouseButton button;
	MouseAction action;

	MouseEventData(Uint32 timestamp, float x, float y, MouseButton button, MouseAction action) : EventData(timestamp) {
		this->x = x;
		this->y = y;
		this->button = button;
		this->action = action;
	}

	const EventType getEventType() override {
		return EventType::MOUSE_EVENT;
	}
};

class KeyEventData : public EventData {
public:
	Sint32 key;
	KeyAction action;
	bool ctrl;
	bool shft;

	KeyEventData(Uint32 timestamp, Sint32 key, KeyAction action, bool ctrl, bool shft) : EventData(timestamp) {
		this->key = key;
		this->action = action;
		this->ctrl = ctrl;
		this->shft = shft;
	}

	const EventType getEventType() override {
		return EventType::KEY_EVENT;
	}
};

class ScriptLoadedData : public EventData {
public:
	unsigned long id;
	LuaScriptPtr script;

	ScriptLoadedData(Uint32 timestamp, unsigned long id, const LuaScriptPtr& script) : EventData(timestamp) {
		this->id = id;
		this->script = script;
	}

	const EventType getEventType() override {
		return EventType::SCRIPT_LOADED;
	}
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
		this->mRemoveList.push_back(std::pair<const EventListenerDelegate&, EventType>(eventDelegate, eventType));
	}

	void update() {
		// remove event listeners in the remove list.
		for (auto it = mRemoveList.begin(); it != mRemoveList.end(); ++it) {
			EventDelegateList& eventDelegates = mEventListeners.at(it->second);
			auto del = std::find_if(eventDelegates.begin(), eventDelegates.end(), [it](EventListenerDelegate val) { return val.id == it->first.id; });
			if (del != eventDelegates.end()) {
				eventDelegates.erase(del);
			}
		}

		while (mQueue.size() > 0) {
			EventDataPtr eventData = mQueue.front();
			mQueue.pop_front();

			const EventType& eventType = eventData->getEventType();
			EventDelegateList eventDelegates = mEventListeners.at(eventType);
			for (auto eventDelegate : eventDelegates) {
				eventDelegate(*eventData);
			}
		}
	}

	void pushEvent(EventData* eventData) {
		EventDataPtr eventDatePtr(eventData);
		mQueue.push_back(eventDatePtr);
	}

private:
	EventDataQueue mQueue;
	EventDataListeners mEventListeners;
	vector<std::pair<const EventListenerDelegate&, EventType>> mRemoveList;

	EventManager() {
		mEventListeners.emplace(EventType::ENTITY_COLLISION_EVENT, EventDelegateList());
		mEventListeners.emplace(EventType::ENTITY_CREATED, EventDelegateList());
		mEventListeners.emplace(EventType::ENTITY_COLLISION_SET, EventDelegateList());
		mEventListeners.emplace(EventType::ENTITY_POSITION_SET, EventDelegateList());
		mEventListeners.emplace(EventType::MAP_LOADED, EventDelegateList());
		mEventListeners.emplace(EventType::SCRIPT_LOADED, EventDelegateList());
		mEventListeners.emplace(EventType::ENTITY_DESTROYED, EventDelegateList());
		mEventListeners.emplace(EventType::ENTITY_ZORDER_SET, EventDelegateList());
		mEventListeners.emplace(EventType::LOAD_ASSET, EventDelegateList());
		mEventListeners.emplace(EventType::MOUSE_EVENT, EventDelegateList());
		mEventListeners.emplace(EventType::KEY_EVENT, EventDelegateList());
	}
};

#endif // !__EVENT_H__
