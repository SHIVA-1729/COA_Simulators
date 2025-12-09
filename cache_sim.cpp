#include <iostream>
#include <vector>
#include <cmath>

// 1. Define what a single "Row" of cache looks like
struct CacheLine {
    bool valid_bit;
    unsigned long long tag; // 64-bit tag to match your architecture
};

// 2. Define the Cache Controller
class Cache {
private:
    std::vector<CacheLine> lines;
    int num_sets;
    int block_size;

public:
    // Constructor: "Builds" the hardware
    Cache(int size_in_bytes, int block_size_in_bytes) {
        this->block_size = block_size_in_bytes;
        // Calculate number of rows (Sets) = Total Size / Block Size
        this->num_sets = size_in_bytes / block_size_in_bytes;
        
        // Initialize the hardware (empty cache)
        lines.resize(num_sets);
        for(int i=0; i<num_sets; i++) {
            lines[i].valid_bit = false; // Power on: Cache is empty
        }

        std::cout << "--- Hardware Configured ---" << std::endl;
        std::cout << "Total Sets (Rows): " << num_sets << std::endl;
    }

    // The Logic: CPU asks for an Address -> Cache checks Hit/Miss
        
void accessMemory(unsigned long long address) {
        unsigned long long index = (address / block_size) % num_sets;
        unsigned long long tag = address / (block_size * num_sets);

        std::cout << "CPU Request: 0x" << std::hex << address << std::dec;
        std::cout << " -> Index: " << index << " | Tag: " << tag;

        if (lines[index].valid_bit == true && lines[index].tag == tag) {
            std::cout << " [HIT] - Data found!" << std::endl;
        } 
        else {
            std::cout << " [MISS] - Bringing data from RAM..." << std::endl;
            lines[index].valid_bit = true;
            lines[index].tag = tag;
        }
    }
    };
int main() {
    // Simulation: 1024 Byte Cache, 16 Byte Blocks
    Cache myCache(1024, 16); 

    // Test with two addresses
    myCache.accessMemory(0x100); // Should map to a specific index
    myCache.accessMemory(0x200); // Should map to a different index
std::cout << "\n--- Step 1: Access 0x100 (First time) ---" << std::endl;
    myCache.accessMemory(0x100); // Should be MISS (Cache is empty)

    std::cout << "\n--- Step 2: Access 0x100 (Again) ---" << std::endl;
    myCache.accessMemory(0x100); // Should be HIT (We just loaded it!)

    std::cout << "\n--- Step 3: Access 0x500 (Collision) ---" << std::endl;
    // 0x500 maps to the SAME index as 0x100 (Index 16).
    // This should overwrite the old data.
    myCache.accessMemory(0x500); // Should be MISS (Conflict)

    std::cout << "\n--- Step 4: Access 0x100 (Old data) ---" << std::endl;
    myCache.accessMemory(0x100); // Should be MISS (It was kicked out by 0x500!)
    return 0;
}

