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

// Enable UTF-8 encoding for Windows console to display Unicode correctly
#ifdef _WIN32
#include <windows.h>
#endif

using namespace std;

// ==============================================================================
// 2. Hardware Definitions (Memory & Registers)
// ==============================================================================
const int MEMORY_SIZE = 4096;
int MEMORY[MEMORY_SIZE] = {0}; // Main Memory (4K words)

// CPU Registers (Basic Computer Architecture)
int AC  = 0;  // Accumulator
int PC  = 0;  // Program Counter
int IR  = 0;  // Instruction Register
int MAR = 0;  // Memory Address Register
int MBR = 0;  // Memory Buffer Register
int E   = 0;  // Extended Accumulator (Carry bit)

// ==============================================================================
// 3. UI & Aesthetics
// ==============================================================================

// ANSI Colors
const string RESET   = "\033[0m";
const string BOLD    = "\033[1m";
const string RED     = "\033[31m";
const string GREEN   = "\033[32m";
const string YELLOW  = "\033[33m";
const string BLUE    = "\033[34m";
const string MAGENTA = "\033[35m";
const string CYAN    = "\033[36m";
const string WHITE   = "\033[37m";

// Main Menu Header
void printMainMenuHeader() {
    cout << BLUE << "\n ╔═══════════════════════════════════════════════╗\n" << RESET;
    cout << BLUE << " ║" << MAGENTA << BOLD << "       [ Computer Architecture Project ]       " << BLUE << "║\n" << RESET;
    cout << BLUE << " ╚═══════════════════════════════════════════════╝\n" << RESET;
}

// Dynamic Module Header (Auto-centers the title)
void printModuleHeader(string title) {
    int padding = 47 - title.length();
    int padLeft = padding / 2;
    int padRight = padding - padLeft;
    
    cout << CYAN << BOLD << "\n ╔═══════════════════════════════════════════════╗\n" << RESET;
    cout << CYAN << BOLD << " ║" << YELLOW << string(padLeft, ' ') << title << string(padRight, ' ') << CYAN << BOLD << "║\n" << RESET;
    cout << CYAN << BOLD << " ╚═══════════════════════════════════════════════╝\n\n" << RESET;
}

// System Alert Messages
void printSuccessMsg(string msg) {
    cout << "\n" << GREEN << BOLD << " [✔] SUCCESS: " << WHITE << msg << "\n" << RESET;
}

void printErrorMsg(string msg) {
    cout << "\n" << RED << BOLD << " ╔═══════════════[ CRITICAL ERROR ]═══════════════╗\n" << RESET;
    cout << RED << BOLD << " ║ " << WHITE << setw(46) << left << msg << RED << BOLD << " ║\n" << RESET;
    cout << RED << BOLD << " ╚════════════════════════════════════════════════╝\n" << RESET;
}



// ==============================================================================
// 4. Utility Functions
// ==============================================================================
void pauseScreen() {
    cout << YELLOW << "\n [Press Enter to return to Main Menu...]" << RESET;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cin.get();
}

// ==============================================================================
// 5. Simulator Core (Fetch, Decode, Execute)
// ==============================================================================
void fetchInstruction() {
    // TODO: Team Member X - Implement Fetch cycle (MAR <- PC, IR <- M[MAR], etc.)
}

void executeInstruction() {
    // TODO: Team Member X - Implement Decode & Execute cycle
}

void runExecutionEngine() {
    // TODO: Team Member X - Loop over Fetch & Execute until HALT
}

// ==============================================================================
// 6. Part 1: Read from Text File
// ==============================================================================
void part1_file() {
    printModuleHeader("Part 1: Load from Text File");
    
    // TODO: Team Member Y - Implement File Reading Logic here
    
    cout << YELLOW << " [!] Module under construction...\n" << RESET;
}

// ==============================================================================
// 7. Part 2: Enter from Screen
// ==============================================================================
void part2_screen() {
    printModuleHeader("Part 2: Manual Screen Input");
    
    // TODO: Team Member Z - Implement Manual Input Logic here
    
    cout << YELLOW << " [!] Module under construction...\n" << RESET;
}

// ==============================================================================
// 8. Part 3: Enter Assembly Mnemonics
// ==============================================================================
void part3_assembly() {
    printModuleHeader("Part 3: Assembly Mnemonics");
    
    // TODO: Team Member W - Implement Assembly parsing and map dictionary here
    
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
            // Clear error flags and ignore bad input
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            choice = 0;
        }

        switch (choice) {
            case 1:
                part1_file();
                pauseScreen();
                break;
            case 2:
                part2_screen();
                pauseScreen();
                break;
            case 3:
                part3_assembly();
                pauseScreen();
                break;
            case 4:
                cout << RED << BOLD << "\n Exiting Simulator. Goodbye!\n\n" << RESET;
                return;
            default:
                printErrorMsg("Invalid choice. Please enter a number between 1 and 4.");
                pauseScreen();
                break;
        }
    }
}

// ==============================================================================
// 10. Main Function (Entry Point)
// ==============================================================================
int main() {
    // Force Windows console to use UTF-8 for Box Drawing characters
    #ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    #endif

    // Reset Memory as a safety measure before starting
    for(int i = 0; i < MEMORY_SIZE; i++) {
       MEMORY[i] = 0;
    }

    // Launch the Simulator
    showMainMenu();

    return 0;
}
hi