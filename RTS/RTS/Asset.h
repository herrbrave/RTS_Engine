#ifndef __ASSET_H__
#define __ASSET_H__

#include"Constants.h"

#include<functional>
#include<memory>
#include<string>
#include<unordered_map>

class Asset;
typedef shared_ptr<Asset> AssetPtr;
typedef weak_ptr<Asset> WeakAssetPtr;

class AssetVendor;
typedef shared_ptr<AssetVendor> AssetVendorPtr;
typedef weak_ptr<AssetVendor> WeakAssetVendorPtr;

class Asset {
public:
	std::string path;
	std::string tag;

	Asset(VoidPtr asset, const std::string& path, const std::string& tag) {
		mAsset = asset;
		this->tag = std::move(tag);
	}

	template<class ClassType>
	shared_ptr<ClassType> getAsset() {
		shared_ptr<void> asset = mAsset;
		shared_ptr<ClassType> converted = static_pointer_cast<ClassType>(asset);
		return converted;
	}

private:
	VoidPtr mAsset;
};

class AssetVendor {
public:
	virtual AssetPtr getAsset(const std::string& tag) = 0;
};

#endif // !__ASSET_H__
