// #include "PrivilegeLevel.h"
#include "PrivilegeLevel.cpp"
#include <iostream>


int main()
{
    std::cout << PrivilegeChecker::GetCurrentPrivilegeLevelString() << std::endl;
    return 0;
}