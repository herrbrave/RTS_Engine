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

class AssetVendor {
public:
	virtual Asset* getAsset(const std::string& tag) = 0;
};

#endif // !__ASSET_H__
