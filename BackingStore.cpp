#include "BackingStore.h"
#include <fstream>
#include <sstream>
#include <iomanip>
#include <iostream>
#include <algorithm>
#include <cctype>

BackingStore::BackingStore(const std::string& filename) : m_filename(filename) {
    load();
}

void BackingStore::initialize(const std::string& filename) {
    if (!sharedInstance) {
        sharedInstance = new BackingStore(filename);
    }
}

BackingStore* BackingStore::getInstance() {
    return sharedInstance;
}

void BackingStore::destroy() {
    if (sharedInstance) {
        delete sharedInstance;
        sharedInstance = nullptr;
    }
}

void BackingStore::write_page(uint32_t pid, size_t pageNumber, const std::vector<std::pair<std::string, uint16_t>> data) {
    m_store[key(pid, pageNumber)] = data;
    persist(); // simple: rewrite entire backing store for consistency
}

std::vector<std::pair<std::string, uint16_t>> BackingStore::read_page(uint32_t pid, size_t pageNumber) {
    auto it = m_store.find(key(pid, pageNumber));
    if (it != m_store.end()) return it->second;
    return {}; // empty means not found
}

bool BackingStore::has_page(uint32_t pid, size_t pageNumber) const {
    return m_store.find(key(pid, pageNumber)) != m_store.end();
}

void BackingStore::load() {
    m_store.clear();
    std::ifstream in(m_filename);
    if (!in) return; // no existing store is fine

    std::string line;
    while (std::getline(in, line)) {
        if (line.rfind("PID", 0) == 0) {
            // Expected format: PID <pid> PAGE <page> SIZE <size>
            std::istringstream header(line);
            std::string tmp;
            uint32_t pid;
            size_t page;
            size_t size;
            header >> tmp >> pid >> tmp >> page >> tmp >> size;
            if (!std::getline(in, line)) break;
            std::vector<std::pair<std::string, uint16_t>> data = from_txt(line);
            m_store[key(pid, page)] = data;
        }
    }
}

void BackingStore::persist() {
    std::ofstream out(m_filename, std::ios::trunc);
    if (!out) return;
    for (const auto& [k, data] : m_store) {
        uint32_t pid = static_cast<uint32_t>(k >> 32);
        size_t page = static_cast<size_t>(k & 0xFFFFFFFF);
        out << "PID " << pid << " PAGE " << page << " SIZE " << data.size() << "\n";
        out << to_txt(data) << "\n";
    }
}

// Hex encode
std::string BackingStore::to_txt(const std::vector<std::pair<std::string, uint16_t>> data) {
    std::ostringstream oss;
    for (auto b : data) {
		oss << b.first << " " << b.second << "\n";
    }
    return oss.str();
}

// Hex decode
std::vector<std::pair<std::string, uint16_t>> BackingStore::from_txt(const std::string& txt) {
    std::vector<std::pair<std::string, uint16_t>> out;

	std::istringstream iss(txt);
	std::string line;
    while (std::getline(iss, line)) {
        std::istringstream lineStream(line);
        std::string varName;
        uint16_t value;
        if (lineStream >> varName >> value) {
            out.emplace_back(varName, value);
        }
	}

    return out;
}
