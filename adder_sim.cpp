#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <cmath>
#include <iomanip>
#include <emscripten/emscripten.h>

using namespace std;

// Helper to format output columns
void printRow(int bitIndex, int a, int b, int c_in, int sum, int c_out, int width) {
    cout << left << setw(width) << bitIndex 
         << setw(width) << a 
         << setw(width) << b 
         << setw(width) << c_in 
         << setw(width) << sum 
         << setw(width) << c_out << endl;
}

int main() {
    cout << "--- UNIVERSAL BINARY ADDER SIMULATOR ---" << endl;

    // 1. Ask for Bit Width
    int n = emscripten_run_script_int("parseInt(prompt('Step 1: Enter Bit Width (N) [e.g. 4, 8]:', '4'))");
    if (isnan(n) || n < 1 || n > 32) n = 4;

    int max_val = pow(2, n) - 1;

    // 2. Ask for Inputs
    string promptA = "Step 2: Enter First Number (A) [0 to " + to_string(max_val) + "]:";
    string promptB = "Step 3: Enter Second Number (B) [0 to " + to_string(max_val) + "]:";
    
    int a_dec = emscripten_run_script_int(("parseInt(prompt('" + promptA + "', '5'))").c_str());
    int b_dec = emscripten_run_script_int(("parseInt(prompt('" + promptB + "', '3'))").c_str());

    cout << "Configuration: " << n << "-bit Adder" << endl;
    cout << "Input A: " << a_dec << endl;
    cout << "Input B: " << b_dec << endl;
    cout << "----------------------------------------" << endl;

    // Store bits in vectors
    vector<int> A(n), B(n), Sum(n), Carry(n + 1);
    
    // Fill vectors from LSB to MSB
    for (int i = 0; i < n; i++) {
        A[i] = (a_dec >> i) & 1;
        B[i] = (b_dec >> i) & 1;
    }

    Carry[0] = 0; // Initial carry is 0

    // Header
    int w = 10; // column width
    cout << left << setw(w) << "Bit Pos" 
         << setw(w) << "A" 
         << setw(w) << "B" 
         << setw(w) << "C-In" 
         << setw(w) << "Sum" 
         << setw(w) << "C-Out" << endl;
    cout << string(w * 6, '-') << endl;

    // Simulate Ripple Carry Adder
    for (int i = 0; i < n; i++) {
        int sum_bit = A[i] ^ B[i] ^ Carry[i];
        int carry_out = (A[i] & B[i]) | (B[i] & Carry[i]) | (Carry[i] & A[i]);
        
        Sum[i] = sum_bit;
        Carry[i + 1] = carry_out;

        printRow(i, A[i], B[i], Carry[i], Sum[i], Carry[i+1], w);
    }
    
    cout << string(w * 6, '-') << endl;

    // Reconstruct Final Result
    int final_sum = 0;
    cout << "Final Carry Output: " << Carry[n] << endl;
    
    cout << "Sum (Binary): ";
    if (Carry[n]) cout << "1"; // Show overflow bit if exists
    for (int i = n - 1; i >= 0; i--) {
        cout << Sum[i];
        if (Sum[i]) final_sum += pow(2, i);
    }
    if (Carry[n]) final_sum += pow(2, n);
    
    cout << "\nSum (Decimal): " << final_sum << endl;

    return 0;
}
