/*
单独编译命令：
g++ -O2 -o ./Test ./Test.cpp -std=c++17
*/
#include <iostream>
#include "AlgorithmModule.hpp"

using namespace AlgorithmModule;

int main()
{
    DigitalEncodingConverter dec;
    auto SM = DigitalEncodingConverter::DigitalEncoding::SignMagnitude;
    auto OC = DigitalEncodingConverter::DigitalEncoding::OnesComplement;
    auto TC = DigitalEncodingConverter::DigitalEncoding::TwosComplement;
    
    std::cout << dec.convert("10000000", TC, SM) << std::endl;  // 00000101
    // std::cout << dec.convert("00000101", SM, TC) << std::endl;  // 00000101
    // std::cout << dec.convert("00000101", TC, OC) << std::endl;  // 00000101
    // std::cout << dec.convert("10011100", SM, OC) << std::endl;  // 1111111101100011
    // std::cout << dec.convert("11100011", OC, TC) << std::endl;  // 1111111111100100
    // std::cout << dec.convert("11100100", TC, SM) << std::endl;  // 0000000011100100
    
    return 0;
}