#include "Core.hpp"
#include <iostream>
int main()
{
    // raiseNullptrException();
    // writeInvalidMemory();
    // readInvalidMemory();
    for (int i = 0; i < 32; i++)
    {
        memoryLeak(1024*1024*1024);
    }
    system("pause");
    return 0;
}