#include <iostream>
#include <vector>
#include <cmath>
#include <iomanip>
#include <emscripten/emscripten.h>

using namespace std;

struct CacheLine {
    bool valid;
    int tag;
    int last_used; // For LRU
};

int main() {
    cout << "--- UNIVERSAL CACHE MEMORY SIMULATOR ---" << endl;

    // 1. CONFIGURATION
    int cache_size = emscripten_run_script_int("parseInt(prompt('Step 1: Enter Cache Size (Number of Lines) [e.g. 8, 16]:', '8'))");
    int block_size = emscripten_run_script_int("parseInt(prompt('Step 2: Enter Block Size (Words per Block) [e.g. 4]:', '4'))");
    int mapping = emscripten_run_script_int("parseInt(prompt('Step 3: Select Mapping (0 = Direct, 1 = Set Associative):', '0'))");
    
    int ways = 1;
    if (mapping == 1) {
        ways = emscripten_run_script_int("parseInt(prompt('Step 3b: Enter Set Associativity (e.g. 2 for 2-way, 4 for 4-way):', '2'))");
    }

    // Calculations
    int num_sets = cache_size / ways;
    int offset_bits = log2(block_size);
    int index_bits = log2(num_sets);
    
    cout << "Configuration:" << endl;
    cout << "  Cache Size: " << cache_size << " lines" << endl;
    cout << "  Set Associativity: " << ways << "-way" << endl;
    cout << "  Number of Sets: " << num_sets << endl;
    cout << "  Offset Bits: " << offset_bits << endl;
    cout << "  Index Bits:  " << index_bits << endl;
    cout << "----------------------------------------" << endl;

    // Initialize Cache
    // Cache is a vector of Sets, where each Set is a vector of CacheLines
    vector<vector<CacheLine>> cache(num_sets, vector<CacheLine>(ways, {false, -1, 0}));
    
    int time_counter = 0;
    int hits = 0, misses = 0;

    // Header
    cout << left << setw(10) << "Access" 
         << setw(10) << "Address" 
         << setw(8) << "Tag" 
         << setw(8) << "Set" 
         << setw(10) << "Result" << endl;
    cout << string(50, '-') << endl;

    // 2. SIMULATION LOOP
    while(true) {
        int addr = emscripten_run_script_int("parseInt(prompt('Enter Memory Address to Access (-1 to Stop):', '-1'))");
        
        if (addr == -1) break; // Exit condition
        
        time_counter++;

        // Address Parsing
        int block_addr = addr / block_size;
        int set_index = block_addr % num_sets;
        int tag = block_addr / num_sets;
        
        bool hit = false;
        int hit_way = -1;

        // Check for HIT
        for (int w = 0; w < ways; w++) {
            if (cache[set_index][w].valid && cache[set_index][w].tag == tag) {
                hit = true;
                hit_way = w;
                cache[set_index][w].last_used = time_counter; // Update LRU
                hits++;
                break;
            }
        }

        string result = "MISS";
        
        if (!hit) {
            misses++;
            // Handle MISS (Placement / Replacement)
            int replace_way = -1;
            
            // 1. Look for empty slot
            for (int w = 0; w < ways; w++) {
                if (!cache[set_index][w].valid) {
                    replace_way = w;
                    break;
                }
            }
            
            // 2. If full, use LRU (Least Recently Used)
            if (replace_way == -1) {
                int min_time = 9999999;
                for (int w = 0; w < ways; w++) {
                    if (cache[set_index][w].last_used < min_time) {
                        min_time = cache[set_index][w].last_used;
                        replace_way = w;
                    }
                }
            }
            
            // Update Cache
            cache[set_index][replace_way].valid = true;
            cache[set_index][replace_way].tag = tag;
            cache[set_index][replace_way].last_used = time_counter;
        } else {
            result = "HIT!";
        }

        // Print Output Row
        cout << left << setw(10) << time_counter 
             << setw(10) << addr 
             << setw(8) << tag 
             << setw(8) << set_index 
             << setw(10) << result << endl;
    }

    cout << string(50, '-') << endl;
    cout << "Simulation Ended." << endl;
    cout << "Total Hits: " << hits << endl;
    cout << "Total Misses: " << misses << endl;
    if (hits + misses > 0)
        cout << "Hit Rate: " << (float)hits / (hits + misses) * 100.0 << "%" << endl;

    return 0;
}
