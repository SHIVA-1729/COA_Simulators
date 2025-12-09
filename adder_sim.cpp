#include <iostream>
#include <vector>
#include <iomanip>
#include <string>
#include <bitset>

using namespace std;

// Bit width for simulation
const int N = 4; 

// Helper: Convert int to binary string
string toBin(int n) {
    return bitset<N>(n).to_string();
}

// ==========================================
// 1. RIPPLE CARRY ADDER (The "Slow" Way)
// ==========================================
void simulateRippleCarry(int A, int B) {
    cout << "\n--- Ripple Carry Adder (RCA) Trace ---" << endl;
    cout << "Bit | A | B | Cin | Sum | Cout | Gate Delay" << endl;
    cout << "---------------------------------------------" << endl;

    int carry = 0;
    int total_delay = 0;
    int result = 0;

    for (int i = 0; i < N; i++) {
        // Extract the i-th bit of A and B
        int a_bit = (A >> i) & 1;
        int b_bit = (B >> i) & 1;

        // Full Adder Logic
        int sum = a_bit ^ b_bit ^ carry;
        int next_carry = (a_bit & b_bit) | (carry & (a_bit ^ b_bit));

        // Visualize the "Ripple" effect
        // Delay accumulates: Each stage waits for the previous carry
        int stage_delay = 2; // Approx 2 gate delays per full adder
        total_delay += stage_delay; 

        cout << " " << i << "  | " << a_bit << " | " << b_bit << " |  " << carry 
             << "  |  " << sum << "  |  " << next_carry << "   | " 
             << total_delay << " ns (Cumulative)" << endl;

        // Store result bit
        if (sum) result |= (1 << i);
        
        // Pass carry to next iteration (The "Ripple")
        carry = next_carry;
    }
    cout << "RCA Result: " << result << " (Total Delay: " << total_delay << " units)" << endl;
}

// ==========================================
// 2. CARRY LOOKAHEAD ADDER (The "Fast" Way)
// ==========================================
void simulateCarryLookahead(int A, int B) {
    cout << "\n--- Carry Lookahead Adder (CLA) Trace ---" << endl;
    cout << "Step 1: Calculate P (Propagate) and G (Generate) in Parallel" << endl;
    
    int P[N], G[N], C[N+1];
    C[0] = 0; // Initial CarryIn

    // Parallel Calculation (Simulated loop)
    for(int i=0; i<N; i++) {
        int a_bit = (A >> i) & 1;
        int b_bit = (B >> i) & 1;
        
        G[i] = a_bit & b_bit; // Generate: Carry is generated if both are 1
        P[i] = a_bit ^ b_bit; // Propagate: Carry is propagated if one is 1
        
        cout << "Bit " << i << ": G=" << G[i] << ", P=" << P[i] << endl;
    }

    cout << "\nStep 2: Calculate Carries using Logic Formula (No Ripple!)" << endl;
    // CLA Logic: C1 = G0 + P0C0
    //            C2 = G1 + P1G0 + P1P0C0 ...
    
    for(int i=0; i<N; i++) {
        C[i+1] = G[i] | (P[i] & C[i]);
        cout << "C" << i+1 << " calculated logic..." << endl;
    }

    // Final Sum Calculation (Parallel)
    int result = 0;
    for(int i=0; i<N; i++) {
        int sum = P[i] ^ C[i];
        if (sum) result |= (1 << i);
    }
    
    cout << "CLA Result: " << result << " (Total Delay: ~3 units CONSTANT)" << endl;
}

int main() {
    int A = 6;  // Binary 0110
    int B = 3;  // Binary 0011
    // Expected Sum: 9 (1001)

    simulateRippleCarry(A, B);
    simulateCarryLookahead(A, B);

    return 0;
}
