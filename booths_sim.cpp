#include <iostream>
#include <iomanip>
#include <bitset>
#include <string>

using namespace std;

// We use 4 bits for visualization (Range: -8 to +7)
const int BITS = 4;

// Helper to print binary
string toBinary(int n) {
    return bitset<BITS>(n).to_string();
}

int main() {
    cout << "--- BOOTH'S MULTIPLICATION SIMULATOR ---" << endl;
    
    // Hardcoded inputs (Try 5 * -3)
    // 5  = 0101
    // -3 = 1101 (2's complement)
    int M = 5;
    int Q = -3;
    
    int A = 0;
    int Q_minus_1 = 0;
    
    cout << "  M (Multiplicand) = " << M << " (" << toBinary(M) << ")" << endl;
    cout << "  Q (Multiplier)   = " << Q << " (" << toBinary(Q) << ")" << endl;
    cout << "--------------------------------------------------------" << endl;
    cout << "| Step |    A     |    Q     | Q-1 | Operation       |" << endl;
    cout << "|------|----------|----------|-----|-----------------|" << endl;
    
    cout << "| Init | " << toBinary(A) << "     | " << toBinary(Q) << "     |  " << Q_minus_1 << "  | Initial State   |" << endl;

    for (int i = 0; i < BITS; i++) {
        // 1. Check Q0 and Q-1
        int Q0 = Q & 1; // Extract LSB
        string op = "No Op           ";

        if (Q0 == 1 && Q_minus_1 == 0) {
            // 10 -> Subtract M
            A = A - M;
            op = "A = A - M       ";
        }
        else if (Q0 == 0 && Q_minus_1 == 1) {
            // 01 -> Add M
            A = A + M;
            op = "A = A + M       ";
        }

        // Handle 4-bit overflow for display (masking)
        // Note: In C++, signed integers handle this logic natively, 
        // but for display we strip the upper bits.
        
        if (op != "No Op           ") {
             cout << "|      | " << toBinary(A) << "     | " << toBinary(Q) << "     |  " << Q_minus_1 << "  | " << op << "|" << endl;
        }

        // 2. Arithmetic Shift Right (ASR)
        // Logic: Shift A, Q, Q-1 as one block
        
        Q_minus_1 = Q & 1; // The bit falling off Q goes to Q-1
        
        // Handle the bit moving from A to Q
        int A_LSB = A & 1;
        Q = (Q >> 1) & 0x7fffffff; // Shift Q (Logic shift)
        if (A_LSB) Q |= (1 << (BITS-1)); // Put A's LSB into Q's MSB

        // Arithmetic Shift A (Preserve Sign)
        int A_Sign = A & (1 << (BITS-1)); // Capture Sign Bit
        A = (A >> 1); // Shift
        if (A_Sign) A |= (1 << (BITS-1)); // Restore Sign Bit
        
        cout << "|  " << i+1 << "   | " << toBinary(A) << "     | " << toBinary(Q) << "     |  " << Q_minus_1 << "  | ASR (Shift)     |" << endl;
        cout << "|------|----------|----------|-----|-----------------|" << endl;
    }

    cout << "\nTrace Complete." << endl;
    return 0;
}
