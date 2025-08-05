//#include <cassert>
//#include <iostream>
//#include <memory>
//#include <print>
//#include "SystemConfig.h"
//#include "BackingStore.h"
//#include "MemoryManager.h"
//#include "Process.h"
//#include "Instruction.h"
//#include "PrintInstruction.h"
//#include "ReadInstruction.h"
//#include "WriteInstruction.h"
//#include "DeclareInstruction.h"
//#include "ConsoleUtil.h"
//
//void test_eviction_behavior() {
//    // Setup small memory: total 512 bytes, frame size 256 → 2 frames
//    SystemConfig::initialize("config.txt");
//
//	BackingStore::initialize("csopesy-backing-store.txt");
//	MemoryManager::initialize();
//
//    // Create P1 with 2 pages
//	std::vector<std::shared_ptr<Instruction>> instructions1; // Empty for this test
//    for (int i = 0; i < 100; ++i) {
//        instructions1.push_back(std::make_shared<PrintInstruction>(PrintType::Hello)); // Dummy instruction
//    }
//    auto p1 = std::make_shared<Process>("P1", instructions1, 512);
//	MemoryManager::getInstance()->addProcess(p1);
//
//    // Access both pages of P1 (simulate reads)
//    MemoryManager::getInstance()->handle_page_fault(*p1, 0, true);
//    MemoryManager::getInstance()->handle_page_fault(*p1, 1, true);
//    assert(MemoryManager::getInstance()->resident_frames_count() == 2);
//
//    // Create P2 with 1 page
//    std::vector<std::shared_ptr<Instruction>> instructions2; // Empty for this test
//    for (int i = 0; i < 100; ++i) {
//        instructions2.push_back(std::make_shared<PrintInstruction>(PrintType::Hello)); // Dummy instruction
//    }
//    auto p2 = std::make_shared<Process>("P2", instructions2, 256);
//	MemoryManager::getInstance()->addProcess(p2);
//
//    // Access P2 page 0 → should cause one eviction
//    MemoryManager::getInstance()->handle_page_fault(*p2, 0, false);
//    assert(MemoryManager::getInstance()->resident_frames_count() == 2);
//    assert(MemoryManager::getInstance()->evictions_clean + MemoryManager::getInstance()->evictions_dirty == 1);
//
//    std::cout << "test_eviction_behavior passed\n";
//}
//
//// Test READ/WRITE instructions trigger page faults and maintain data correctly
//void test_read_write_behavior() {
//    SystemConfig::initialize("config.txt");
//
//	BackingStore::initialize("csopesy-backing-store.txt");
//	MemoryManager::initialize();
//
//    // Prepare instructions: WRITE 0x00 0xABCD; READ var1 0x00
//    std::vector<std::shared_ptr<Instruction>> instrs;
//	instrs.push_back(std::make_shared<WriteInstruction>("0x00", 43981)); // 0xABCD = 43981 decimal
//    instrs.push_back(std::make_shared<ReadInstruction>("var1", "0x00"));
//
//    // Create process and register
//    auto p = std::make_shared<Process>("PRW", instrs, 128);
//	MemoryManager::getInstance()->addProcess(p);
//
//    // Reset stats
//    MemoryManager::getInstance()->total_pages_in = MemoryManager::getInstance()->total_pages_out = MemoryManager::getInstance()->evictions_clean = MemoryManager::getInstance()->evictions_dirty = 0;
//
//    // Execute instructions
//    for (int i = 0; i < static_cast<int>(instrs.size()); ++i) {
//        p->executeInstruction(0);
//    }
//
//    // After execution, page 0 should have been faulted in once for WRITE,
//    // and WRITE marks it dirty. The READ should hit the page, not fault again.
//    assert(p->page_faults == 1);
//    assert(MemoryManager::getInstance()->total_pages_in == 1);
//    assert(MemoryManager::getInstance()->evictions_clean + MemoryManager::getInstance()->evictions_dirty == 0);
//
//    // Verify variable was set and read correctly
//    uint16_t value = 0;
//	bool found = p->getVariable("var1");
//    assert(found);
//	assert(p->getVariable("var1") == 43981);
//
//    std::cout << "test_read_write_behavior passed\n";
//}
//
//void test_full_memory_page_fault() {
//    // 1. Small memory: 256 bytes total, 128-byte frames → 2 frames
//	SystemConfig::initialize("config.txt");
//
//    BackingStore::initialize("csopesy-backing-store.txt");
//    MemoryManager::initialize();
//
//    // 2. Process P1 with 2 pages (256 bytes)
//    auto p1 = std::make_shared<Process>("P1", std::vector<std::shared_ptr<Instruction>>{}, 256);
//	MemoryManager::getInstance()->addProcess(p1);
//
//    // Fault in both pages of P1 → fills both frames, no eviction yet
//    MemoryManager::getInstance()->handle_page_fault(*p1, 0, false);
//    MemoryManager::getInstance()->handle_page_fault(*p1, 1, false);
//    assert(MemoryManager::getInstance()->resident_frames_count() == 2);
//
//    // 3. Process P2 with 1 page (128 bytes)
//    auto p2 = std::make_shared<Process>("P2",std::vector<std::shared_ptr<Instruction>>{}, 128);
//	MemoryManager::getInstance()->addProcess(p2);
//
//    // Fault in P2 page 0 → no free frames, should evict P1’s oldest page (vpn 0)
//    MemoryManager::getInstance()->handle_page_fault(*p2, 0, false);
//
//    // Still 2 resident frames (one eviction + one load)
//    assert(MemoryManager::getInstance()->resident_frames_count() == 2);
//    // Exactly one eviction occurred
//    assert((MemoryManager::getInstance()->evictions_clean + MemoryManager::getInstance()->evictions_dirty) == 1);
//
//    // Verify FIFO: P1’s vpn 0 was invalidated, vpn 1 still present
//    auto& pte10 = p1->page_table_at(0);
//    auto& pte11 = p1->page_table_at(1);
//    assert(!pte10.present);
//    assert(pte11.present);
//
//    // And P2’s page 0 is now resident
//    assert(p2->page_table_at(0).present);
//
//    std::cout << "test_full_memory_page_fault passed\n";
//}
//
//int main() {
//    //test_eviction_behavior();
//	//test_read_write_behavior();
//	//test_full_memory_page_fault();
//    std::cout << "All tests passed\n";
//    return 0;
//}

#include <Windows.h>

#include "Console.h"

int main() {
    SetConsoleOutputCP(CP_UTF8);

    ConsoleSystem::initialize();

    bool isRunning = true;
    while (isRunning) {
        ConsoleSystem::getInstance()->drawConsole();
        ConsoleSystem::getInstance()->processInput();

        isRunning = ConsoleSystem::getInstance()->isRunning();
    }

    ConsoleSystem::destroy();

    return 0;
}