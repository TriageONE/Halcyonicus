#include <string>
#include <iostream>

using namespace std;

int main() {

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
    //Does not work for no good reason
    #include <windows.h>
    //_setmode(_fileno(stdout), _O_U16TEXT);
    // Enable buffering to prevent VS from chopping up UTF-8 byte sequences
    setvbuf(stdout, nullptr, _IOFBF, 2000);
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD mode;
    GetConsoleMode(hConsole, &mode);
    mode |= 0x0004; // Enable ANSI escape codes
    SetConsoleMode(hConsole, mode);

    for (int i = 0; i < 256; i++) {
        cout << "\x1b[38;5;" << i << "m" << "Hello, world!" << endl;
    }
#endif


    return 0;

}
