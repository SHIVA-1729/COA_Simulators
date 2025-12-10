#include <iostream>
#include <bitset>
#include <string>
#include <vector>
#include <cmath>
#include <iomanip> // REQUIRED for tidy tables
#include <emscripten/emscripten.h>

using namespace std;

// Helper to chop a 32-bit binary string down to 'n' bits
string toBinary(int val, int n) {
    string s = bitset<32>(val).to_string();
    return s.substr(32 - n);
}

string add(string a, string b) {
    string res = "";
    int s = 0;
    int len = a.length();
    for (int i = len - 1; i >= 0; i--) {
        int bit1 = a[i] - '0';
        int bit2 = b[i] - '0';
        int sum = bit1 + bit2 + s;
        res = to_string(sum % 2) + res;
        s = sum / 2;
    }
    return res;
}

void asr(string &ac, string &q, int &qn1) {
    int len = ac.length();
    qn1 = q[len - 1] - '0'; 
    for (int i = len - 1; i > 0; i--) q[i] = q[i - 1];
    q[0] = ac[len - 1]; 
    char msb = ac[0];
    for (int i = len - 1; i > 0; i--) ac[i] = ac[i - 1];
    ac[0] = msb; 
}

// FORMATTING FIX: Fixed width columns instead of tabs
void printRow(int step, string ac, string q, int qn1, string op, int width) {
    cout << left << setw(8) << step 
         << setw(width) << ac 
         << setw(width) << q 
         << setw(8) << qn1 
         << op << endl;
}

int main() {
    cout << "--- UNIVERSAL BOOTH'S SIMULATOR ---" << endl;
    
    // 1. INPUTS
    int n = emscripten_run_script_int("parseInt(prompt('Step 1: Enter Number of Bits (N) [e.g. 4, 5, 8]:', '4'))");
    if (isnan(n) || n < 2 || n > 32) n = 4;

    int min_val = -pow(2, n-1);
    int max_val = pow(2, n-1) - 1;

    string promptM = "Step 2: Enter Multiplicand (M) [" + to_string(min_val) + " to " + to_string(max_val) + "]:";
    string promptQ = "Step 3: Enter Multiplier (Q) [" + to_string(min_val) + " to " + to_string(max_val) + "]:";
    
    int m_in = emscripten_run_script_int(("parseInt(prompt('" + promptM + "', '5'))").c_str());
    int q_in = emscripten_run_script_int(("parseInt(prompt('" + promptQ + "', '-3'))").c_str());

    // 2. BINARY CONVERSION DISPLAY (Requested Feature)
    string m = toBinary(m_in, n);
    string q = toBinary(q_in, n);
    string neg_m = toBinary(-m_in, n);
    
    cout << "----------------------------------------" << endl;
    cout << "Decimal Inputs:" << endl;
    cout << "  M  = " << m_in << endl;
    cout << "  Q  = " << q_in << endl;
    cout << endl;
    cout << "Binary Equivalents (" << n << "-bit):" << endl;
    cout << "  M  = " << m << endl;
    cout << " -M  = " << neg_m << " (2's Complement)" << endl;
    cout << "  Q  = " << q << endl;
    cout << "----------------------------------------" << endl;

    // 3. TABLE SETUP
    string ac = "";
    for(int k=0; k<n; k++) ac += "0";
    int qn1 = 0;
    
    // Dynamic formatting: Column width depends on N (so it doesn't break for 8 bits)
    int col_width = max(n + 4, 10); 

    // Header
    cout << left << setw(8) << "Step" 
         << setw(col_width) << "AC" 
         << setw(col_width) << "Q" 
         << setw(8) << "Q-1" 
         << "Operation" << endl;
         
    cout << string(8 + col_width*2 + 8 + 15, '-') << endl; // Separator line

    printRow(0, ac, q, qn1, "Initialization", col_width);

    for (int i = 0; i < n; i++) {
        int q0 = q[n - 1] - '0';
        
        if (q0 == 1 && qn1 == 0) {
            ac = add(ac, neg_m);
            printRow(i + 1, ac, q, qn1, "A = A - M", col_width);
        }
        else if (q0 == 0 && qn1 == 1) {
            ac = add(ac, m);
            printRow(i + 1, ac, q, qn1, "A = A + M", col_width);
        }
        
        asr(ac, q, qn1); 
        printRow(i + 1, ac, q, qn1, "ASR (Shift)", col_width);
        cout << endl; // Empty line between steps for clarity
    }

    string resultStr = ac + q;
    cout << string(8 + col_width*2 + 8 + 15, '-') << endl;
    cout << "Final Result (Binary): " << resultStr << endl;
    
    // Decimal conversion
    unsigned long resVal = 0;
    unsigned long powerOfTwo = 1;
    for(int i = resultStr.length()-1; i >= 0; i--) {
        if(resultStr[i] == '1') resVal += powerOfTwo;
        powerOfTwo *= 2;
    }
    long long finalDec = resVal;
    long long max_pos_val = pow(2, 2*n - 1);
    if (resVal >= max_pos_val) {
        finalDec = (long long)resVal - (long long)pow(2, 2*n);
    }
    
    cout << "Final Result (Decimal): " << finalDec << endl;
    
    return 0;
}
