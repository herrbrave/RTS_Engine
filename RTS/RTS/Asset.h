#ifndef __ASSET_H__
#define __ASSET_H__

#include<functional>
#include<memory>
#include<string>
#include<unordered_map>

class Asset {
public:
	std::string path;
	std::string tag;

	Asset(void* asset, const std::string& path, const std::string& tag, std::function<void(void*)> assetDeleter) {
		mAsset = asset;
		this->tag = std::move(tag);
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

	void deregisterAsset(const std::string& assetTag) {
		mAssets.erase(mAssets.find(assetTag));
	}

	Asset* getAsset(const std::string& assetTag) {
		return mAssets[assetTag];
	}

	bool contains(const std::string& assetTag) {
		return (mAssets.find(assetTag) != mAssets.end());
	}

private:
	std::unordered_map<std::string, Asset*> mAssets;
};

#endif // !__ASSET_H__
