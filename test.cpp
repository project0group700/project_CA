// ==============================================================================
// 1. Standard Libraries
// ==============================================================================
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <iomanip>
#include <limits>
#include <cstdlib>
#include <unordered_map>
#include <sstream>

// Enable UTF-8 encoding for Windows console
#ifdef _WIN32
#include <windows.h>
#endif

using namespace std;

// ==============================================================================
// 2. Hardware Definitions (Memory & Registers)
// ==============================================================================
const int MEMORY_SIZE = 4096;
int MEMORY[MEMORY_SIZE] = {0}; // Main Memory (4K words)

// CPU Registers
int AC  = 0;  // Accumulator
int PC  = 0;  // Program Counter
int IR  = 0;  // Instruction Register
int MAR = 0;  // Memory Address Register
int MBR = 0;  // Memory Buffer Register

// I/O Registers & Flags
int INPR = 0; // Input Register
int OUTR = 0; // Output Register
int E   = 0;  // Extended Accumulator (Carry bit)
int IEN = 0;  // Interrupt Enable
int FGI = 0;  // Input Flag
int FGO = 0;  // Output Flag

// System Flags
bool halt_flag = false;

// ==============================================================================
// 3. UI & Aesthetics
// ==============================================================================
const string RESET   = "\033[0m";
const string BOLD    = "\033[1m";
const string RED     = "\033[31m";
const string GREEN   = "\033[32m";
const string YELLOW  = "\033[33m";
const string BLUE    = "\033[34m";
const string MAGENTA = "\033[35m";
const string CYAN    = "\033[36m";
const string WHITE   = "\033[37m";

void printMainMenuHeader() {
    cout << BLUE << "\n ╔═══════════════════════════════════════════════╗\n" << RESET;
    cout << BLUE << " ║" << MAGENTA << BOLD << "       [ Computer Architecture Project ]       " << BLUE << "║\n" << RESET;
    cout << BLUE << " ╚═══════════════════════════════════════════════╝\n" << RESET;
}

void printModuleHeader(string title) {
    int padding = 47 - title.length();
    int padLeft = padding / 2;
    int padRight = padding - padLeft;
    cout << CYAN << BOLD << "\n ╔═══════════════════════════════════════════════╗\n" << RESET;
    cout << CYAN << BOLD << " ║" << YELLOW << string(padLeft, ' ') << title << string(padRight, ' ') << CYAN << BOLD << "║\n" << RESET;
    cout << CYAN << BOLD << " ╚═══════════════════════════════════════════════╝\n\n" << RESET;
}

void printSuccessMsg(string msg) { cout << "\n" << GREEN << BOLD << " [✔] SUCCESS: " << WHITE << msg << "\n" << RESET; }
void printErrorMsg(string msg) {
    cout << "\n" << RED << BOLD << " ╔═══════════════[ CRITICAL ERROR ]═══════════════╗\n" << RESET;
    cout << RED << BOLD << " ║ " << WHITE << setw(46) << left << msg << RED << BOLD << " ║\n" << RESET;
    cout << RED << BOLD << " ╚════════════════════════════════════════════════╝\n" << RESET;
}

// ==============================================================================
// 4. Utility Functions
// ==============================================================================
void pauseScreen() {
    cout << YELLOW << "\n [Press Enter to continue...]" << RESET;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cin.get();
}

// ==============================================================================
// 5. Simulator Core (Fetch, Decode, Execute)
// ==============================================================================
void fetchInstruction() {
    MAR = PC;
    IR = MEMORY[MAR];
    PC = (PC + 1) & 0x0FFF; // 12-bit PC
}

void executeInstruction() {
    int opcode = (IR & 0x7000) >> 12; 
    int i_bit = (IR & 0x8000) >> 15;  
    int address = IR & 0x0FFF;        

    if (opcode == 7) { 
        if (i_bit == 0) { // Register Reference
            switch (IR) {
                case 0x7800: AC = 0; break;
                case 0x7400: E = 0; break;
                case 0x7200: AC = (~AC) & 0xFFFF; break;
                case 0x7100: E = (~E) & 1; break;
                case 0x7080: { int next_E = AC & 1; AC = (AC >> 1) | (E << 15); E = next_E; } break;
                case 0x7040: { int next_E = (AC & 0x8000) >> 15; AC = ((AC << 1) & 0xFFFF) | E; E = next_E; } break;
                case 0x7020: AC = (AC + 1) & 0xFFFF; break;
                case 0x7010: if ((AC & 0x8000) == 0) PC = (PC + 1) & 0x0FFF; break;
                case 0x7008: if ((AC & 0x8000) != 0) PC = (PC + 1) & 0x0FFF; break;
                case 0x7004: if (AC == 0) PC = (PC + 1) & 0x0FFF; break;
                case 0x7002: if (E == 0) PC = (PC + 1) & 0x0FFF; break;
                case 0x7001: halt_flag = true; cout << RED << " [!] HALT Instruction Reached.\n" << RESET; break;
                default: cout << YELLOW << " [!] Unknown Register Instruction\n" << RESET;
            }
        } else { // I/O Reference
            switch (IR) {
                case 0xF800: cout << CYAN << " INPUT > " << RESET; char c; cin >> c; INPR = (int)c; AC = (AC & 0xFF00) | INPR; FGI = 0; break;
                case 0xF400: OUTR = AC & 0x00FF; cout << CYAN << " OUTPUT > " << (char)OUTR << "\n" << RESET; FGO = 0; break;
                case 0xF200: if (FGI == 1) PC = (PC + 1) & 0x0FFF; break;
                case 0xF100: if (FGO == 1) PC = (PC + 1) & 0x0FFF; break;
                case 0xF080: IEN = 1; break;
                case 0xF040: IEN = 0; break;
                default: cout << YELLOW << " [!] Unknown I/O Instruction\n" << RESET;
            }
        }
    } 
    else { // Memory Reference
        int effective_address = (i_bit == 1) ? MEMORY[address] : address;
        switch (opcode) {
            case 0: AC = AC & MEMORY[effective_address]; break;
            case 1: { long temp = (long)AC + MEMORY[effective_address]; AC = temp & 0xFFFF; E = (temp > 0xFFFF) ? 1 : 0; } break;
            case 2: AC = MEMORY[effective_address]; break;
            case 3: MEMORY[effective_address] = AC; break;
            case 4: PC = effective_address; break;
            case 5: MEMORY[effective_address] = PC; PC = (effective_address + 1) & 0x0FFF; break;
            case 6: MEMORY[effective_address] = (MEMORY[effective_address] + 1) & 0xFFFF; if (MEMORY[effective_address] == 0) PC = (PC + 1) & 0x0FFF; break;
        }
    }
}

void runExecutionEngine() {
    printModuleHeader("Execution Engine (CPU)");
    PC = 0; 
    halt_flag = false;
    cout << GREEN << " --- Starting CPU Execution ---\n" << RESET;
    
    while (!halt_flag) {
        fetchInstruction();
        executeInstruction();
        
        // Print AC state for debugging/visibility
        if(!halt_flag) {
            cout << WHITE << " [PC: " << setw(3) << setfill('0') << hex << uppercase << PC 
                 << "] AC = " << setw(4) << setfill('0') << AC << dec << "\n" << RESET;
        }
        
        if (PC >= MEMORY_SIZE) { printErrorMsg("PC exceeded memory bounds!"); break; }
    }
    cout << GREEN << "\n --- Execution Finished ---\n" << RESET;
}

// ==============================================================================
// 6. Part 1: Read from Text File
// ==============================================================================
// ==============================================================================
// 6. Part 1: Read from Text File
// ==============================================================================
void part1_file() {
    printModuleHeader("Part 1: Load from Text File");
    string filename;
    cout << YELLOW << " Enter filename (e.g., instructions.txt): " << RESET;
    cin >> filename;

    ifstream inputFile(filename);
    string instruction;

    if (inputFile.is_open()) {
        int address = 0;
        cout << GREEN << "\n [✔] Reading instructions...\n" << RESET;
        
        while (getline(inputFile, instruction)) {
            if (instruction.empty()) continue;
            int parsed_value = 0;
            try {
                if (instruction.length() == 16) parsed_value = stoi(instruction, nullptr, 2);
                else parsed_value = stoi(instruction, nullptr, 16);

                MEMORY[address] = parsed_value;

                // --- استخراج الوصف (مدمج داخل الدالة للحفاظ على الهيكل) ---
                int instr = MEMORY[address];
                int opcode = (instr & 0x7000) >> 12;
                int i_bit = (instr & 0x8000) >> 15;
                int d1 = (instr & 0xF000) >> 12;
                string desc = "Data / Unknown"; // القيمة الافتراضية لو كان مجرد رقم

                if (d1 == 7) {
                    if (i_bit == 0) {
                        switch (instr) {
                            case 0x7800: desc = "CLA: Clear AC"; break;
                            case 0x7400: desc = "CLE: Clear E"; break;
                            case 0x7200: desc = "CMA: Complement AC"; break;
                            case 0x7100: desc = "CME: Complement E"; break;
                            case 0x7080: desc = "CIR: Circulate right"; break;
                            case 0x7040: desc = "CIL: Circulate left"; break;
                            case 0x7020: desc = "INC: Increment AC"; break;
                            case 0x7010: desc = "SPA: Skip if positive"; break;
                            case 0x7008: desc = "SNA: Skip if negative"; break;
                            case 0x7004: desc = "SZA: Skip if AC is zero"; break;
                            case 0x7002: desc = "SZE: Skip if E is zero"; break;
                            case 0x7001: desc = "HLT: Halt computer"; break;
                        }
                    } else {
                        switch (instr) {
                            case 0xF800: desc = "INP: Input char"; break;
                            case 0xF400: desc = "OUT: Output char"; break;
                            case 0xF200: desc = "SKI: Skip on input"; break;
                            case 0xF100: desc = "SKO: Skip on output"; break;
                            case 0xF080: desc = "ION: Interrupt on"; break;
                            case 0xF040: desc = "IOF: Interrupt off"; break;
                        }
                    }
                } else {
                    string mode = (i_bit == 1) ? " (Indirect)" : " (Direct)";
                    switch (opcode) {
                        case 0: desc = "AND" + mode; break;
                        case 1: desc = "ADD" + mode; break;
                        case 2: desc = "LDA" + mode; break;
                        case 3: desc = "STA" + mode; break;
                        case 4: desc = "BUN" + mode; break;
                        case 5: desc = "BSA" + mode; break;
                        case 6: desc = "ISZ" + mode; break;
                    }
                }
                // ---------------------------------------------------------

                cout << WHITE << " Stored in [" << right << setw(3) << setfill('0') << hex << uppercase << address << "] -> " 
                     << setw(4) << setfill('0') << MEMORY[address] 
                     << CYAN << "  |  " << desc << dec << "\n" << RESET;
                
                address++;
                if (address >= MEMORY_SIZE) break;
            } catch (...) {
                printErrorMsg("Invalid format: " + instruction);
            }
        }
        inputFile.close();
        printSuccessMsg("Loaded into memory.");
        
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        pauseScreen();
        
        runExecutionEngine();
        
    } else {
        printErrorMsg("Unable to open the file!");
    }
}
// ==============================================================================
// 7. Part 2: Enter from Screen
// ==============================================================================
void part2_screen() {
    printModuleHeader("Part 2: Manual Screen Input");
    cout << YELLOW << " [!] Module under construction...\n" << RESET;
}

// ==============================================================================
// 8. Part 3: Enter Assembly Mnemonics
// ==============================================================================
void part3_assembly() {
    printModuleHeader("Part 3: Assembly Mnemonics");
    cout << YELLOW << " [!] Module under construction...\n" << RESET;
}

// ==============================================================================
// 9. Main Menu
// ==============================================================================
void showMainMenu() {
    int choice = 0;
    while (true) {
        printMainMenuHeader();
        cout << GREEN << "  1. " << WHITE << "Read from Text File (HEX/Binary)\n";
        cout << GREEN << "  2. " << WHITE << "Enter from Screen (HEX/Binary)\n";
        cout << GREEN << "  3. " << WHITE << "Enter Assembly Mnemonics\n";
        cout << RED   << "  4. " << WHITE << "Exit Simulator\n";
        cout << BLUE  << " ───────────────────────────────────────────────\n" << RESET;
        cout << YELLOW << "  Enter your choice (1-4): " << RESET;

        if (!(cin >> choice)) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            choice = 0;
        }

        switch (choice) {
            case 1: part1_file(); pauseScreen(); break;
            case 2: part2_screen(); pauseScreen(); break;
            case 3: part3_assembly(); pauseScreen(); break;
            case 4: cout << RED << BOLD << "\n Exiting Simulator. Goodbye!\n\n" << RESET; return;
            default: printErrorMsg("Invalid choice."); pauseScreen(); break;
        }
    }
}

// ==============================================================================
// 10. Main Function (Entry Point)
// ==============================================================================
int main() {
    #ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    #endif

    for(int i = 0; i < MEMORY_SIZE; i++) MEMORY[i] = 0;
    showMainMenu();
    return 0;
}