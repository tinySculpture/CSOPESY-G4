#pragma once
#include <string>
#include <unordered_map>
#include <vector>
#include <cstdint>

class BackingStore {
public:

	static void initialize(const std::string& filename);

    static BackingStore* getInstance();

    static void destroy();

    void write_page(uint32_t pid, size_t pageNumber, const std::vector<std::pair<std::string, uint16_t>> data);
    std::vector<std::pair<std::string, uint16_t>> read_page(uint32_t pid, size_t pageNumber);
    bool has_page(uint32_t pid, size_t pageNumber) const;

    // Load existing backing store entries from disk at startup
    void load();
    void persist(); // writes the in-memory map to disk

private:
    BackingStore(const std::string& filename);

	inline static BackingStore* sharedInstance = nullptr;

    std::string m_filename;
    std::unordered_map < uint64_t, std::vector < std::pair < std::string , uint16_t >> > m_store;

    static uint64_t key(uint32_t pid, size_t pageNumber) {
        return (static_cast<uint64_t>(pid) << 32) | static_cast<uint64_t>(pageNumber);
    }

    // Helpers for hex encoding/decoding
    static std::string to_txt(const std::vector<std::pair<std::string, uint16_t>> data);
    static std::vector<std::pair<std::string, uint16_t>> from_txt(const std::string& hex);
};
