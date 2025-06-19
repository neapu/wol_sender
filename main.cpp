#include <iostream>
#include "WolSender.h"
#ifdef DDEBUG
#define DEBUG_MAC_ADDR ""
#define DEBUG_TARGET_IP ""
#endif

void printUsage(const char* programName) {
    std::cerr << "Usage: " << programName << " <MAC_ADDRESS> [TARGET_IP]" << std::endl;
    std::cerr << "  MAC_ADDRESS: 目标设备MAC地址 (格式: AA:BB:CC:DD:EE:FF 或 AA-BB-CC-DD-EE-FF 或 AABBCCDDEEFF)" << std::endl;
    std::cerr << "  TARGET_IP: 可选，目标设备IP地址，提供后程序将等待设备上线，超时时间为60秒" << std::endl;
}

int main(int argc, char *argv[])
{
    const char *macAddress = nullptr;
    const char *targetIp = nullptr;

#ifdef DDEBUG
    macAddress = DEBUG_MAC_ADDR;
    targetIp = DEBUG_TARGET_IP;
    std::cout << "Debug mode is enabled. Using MAC address: " << macAddress << std::endl;
    if (targetIp && strlen(targetIp) > 0) {
        std::cout << "Will wait for host " << targetIp << " to come online." << std::endl;
    }
#else
    if (argc < 2) {
        printUsage(argv[0]);
        return 1;
    }

    macAddress = argv[1];
    std::cout << "Using MAC address: " << macAddress << std::endl;

    // 检查是否提供了目标IP
    if (argc > 2) {
        targetIp = argv[2];
        std::cout << "Will wait for host " << targetIp << " to come online." << std::endl;
    }
#endif

    // 创建WolSender对象，传入MAC地址和目标IP（如果有）
    return WolSender(macAddress, targetIp ? targetIp : "").run();
}
