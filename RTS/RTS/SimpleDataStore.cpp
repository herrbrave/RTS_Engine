#include"SimpleDataStore.h"

void SimpleDataStore::pushData(const std::string& key, const std::string& val) {
	this->data.emplace(key, val);
}

std::string& SimpleDataStore::getData(const std::string& key) {
	return this->data.at(key);
}

void SimpleDataStore::flush() {
	this->data.clear();
}

void SimpleDataStore::save(const std::string& path) {

	std::fstream file(path.empty() ? this->datePath : path, std::fstream::out | std::fstream::trunc);
	if (!file.is_open()) {
		ERR("Unable to open file: " + path);
	}

	for (auto it = this->data.begin(); it != this->data.end(); it++) {
		file << it->first + "," + it->second << std::endl;
	}

	file.close();
}


void SimpleDataStore::load(const std::string& path) {
	flush();

	std::fstream file(path);
	if (!file.is_open()) {
		ERR("Unable to open file: " + path);
	}

	std::string line;
	while (std::getline(file, line)) {
		this->data.emplace(line.substr(0, line.find(",")), line.substr(line.find(",") + 2, line.size()));
	}

	file.close();
}