#ifndef __ASSET_H__
#define __ASSET_H__

#include<functional>
#include<memory>
#include<string>
#include<unordered_map>

class Asset {
public:
	std::string tag;

	Asset(void* asset, std::string tag, std::function<void(void*)> assetDeleter) {
		mAsset = asset;
		tag = std::move(tag);
	}
	
	~Asset() {
		mDeleter(mAsset);
	}

	void* getAsset() {
		return mAsset;
	}

private:
	void* mAsset;
	std::function<void(void*)> mDeleter;
};

class AssetSystem {
public:
	void registerAsset(Asset* asset) {
		mAssets.emplace(asset->tag, asset);
	}

	void deregisterAsset(std::string tag) {
		mAssets.erase(mAssets.find(tag));
	}

	Asset* getAsset(std::string tag) {
		return mAssets.at(tag);
	}

private:
	std::unordered_map<std::string, Asset*> mAssets;
};

#endif // !__ASSET_H__
