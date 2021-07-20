#pragma once

#include"Constants.h"

#include<iostream>
#include<fstream>
#include<string>

class SimpleDataStore;
typedef shared_ptr<SimpleDataStore> SimpleDataStorePtr;
typedef weak_ptr<SimpleDataStore> WeakSimpleDataStorePtr;

class SimpleDataStore {
public:
	unordered_map<std::string, std::string> data;

	SimpleDataStore(const string& path) {
		this->datePath = path;
		std::fstream file(path);
		if (file.good()) {
			this->load(path);
		}
	}

	void pushData(const std::string& key, const std::string& val);
	std::string& getData(const std::string& key);
	void flush();
	void save(const std::string& path = "");
private:

	std::string datePath;

	void load(const std::string& path);
};

