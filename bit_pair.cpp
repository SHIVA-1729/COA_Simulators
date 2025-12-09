#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include <bitset>

using namespace std;

// Function to get the recoded value for a 3-bit group
string decodeGroup(int three_bits) {
    // The Input is a 3-bit integer (0 to 7) representing [Q(i+1), Q(i), Q(i-1)]
    // Logic Table from Morris Mano / Syllabus:
    switch (three_bits) {
        case 0: return " 0 "; // 000
        case 1: return "+1 "; // 001
        case 2: return "+1 "; // 010
        case 3: return "+2 "; // 011 -> 2M
        case 4: return "-2 "; // 100 -> -2M
        case 5: return "-1 "; // 101
        case 6: return "-1 "; // 110
        case 7: return " 0 "; // 111
        default: return " ? ";
    }
}

int main() {
    // Input: A 16-bit multiplier (Example: -6)
    // Binary: 1111 1111 1111 1010
    int Q = -6; 
    
    // We simulate a 8-bit registers for visualization
    // 8-bit Binary of -6: 1111 1010
    // We usually pad with 0s to make it work for bit-pair grouping
    
    vector<int> bits;
    // Load bits of Q into a vector (LSB first) + "Phantom Zero" at start
    bits.push_back(0); // Q(-1) is always 0 initially
    
    // Extract 8 bits
    for(int i=0; i<8; i++) {
        bits.push_back((Q >> i) & 1);
    }
    // Pad extra 0s if needed for grouping (we need groups of 3, stepping by 2)
    // Current size: 9. We are good for 4 groups (indices 0,2,4,6 base)

    cout << "\n--- BIT-PAIR RECODING SIMULATOR (8-bit) ---" << endl;
    cout << "Multiplier Q = " << Q << endl;
    cout << "Binary Q     = ";
    for(int i=8; i>=1; i--) cout << bits[i];
    cout << "\n-------------------------------------------" << endl;
    cout << "| Group (3-bits) | Recoded Op | Explanation |" << endl;
    cout << "-------------------------------------------" << endl;

    // Scan step by 2
    for(int i=0; i<8; i+=2) {
        // Form the 3-bit group: [Q(i+1), Q(i), Q(i-1)]
        // Note: In our vector, bits[0] is Q(-1), bits[1] is Q(0)...
        // So group for step i is: bits[i+2], bits[i+1], bits[i]
        
        int b2 = bits[i+2]; // MSB of group
        int b1 = bits[i+1];
        int b0 = bits[i];   // LSB of group (Q-1 logic)
        
        int val = (b2 << 2) | (b1 << 1) | b0;
        
        string op = decodeGroup(val);
        
        cout << "|  " << b2 << b1 << b0 << " (Bits " << i+1 << "-" << i-1 << ")  |    " 
             << op << "     | ";
             
        if (op == "+1 ") cout << "Add 1 * M";
        else if (op == "+2 ") cout << "Add 2 * M (Shift M left)";
        else if (op == "-1 ") cout << "Sub 1 * M";
        else if (op == "-2 ") cout << "Sub 2 * M (Shift M left)";
        else cout << "Do Nothing";
        
        cout << endl;
    }
    cout << "-------------------------------------------" << endl;
    
    return 0;
}
