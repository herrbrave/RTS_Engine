#ifndef __SYSTEM_MANAGER_H__
#define __SYSTEM_MANAGER_H__

#include"Asset.h"
#include"Graphics.h"
#include"Entity.h"

#include<memory>

class SystemManager {
public:
	std::unique_ptr<AssetSystem> assetSystem{ nullptr };
	std::unique_ptr<EntitySystem> entitySystem{ nullptr };
	std::unique_ptr<PhysicsSystem> physicsSystem{ nullptr };
	std::unique_ptr<GraphicsSystem> graphicsSystem{ nullptr };

	SystemManager(GraphicsConfig* graphicsConfig) {
		assetSystem.reset(new AssetSystem());
		entitySystem.reset(new EntitySystem());
		physicsSystem.reset(new PhysicsSystem());
		graphicsSystem.reset(new GraphicsSystem(assetSystem.get(), graphicsConfig, physicsSystem.get()));
	}
};

#endif // !__SYSTEM_MANAGER_H__
