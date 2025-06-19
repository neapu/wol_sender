#include <iostream>
#include "WolSender.h"
#ifdef DDEBUG
#define DEBUG_MAC_ADDR ""
#endif

int main(int argc, char *argv[])
{
#ifdef DDEBUG
    const char *macAddress = DEBUG_MAC_ADDR;
    std::cout << "Debug mode is enabled. Using MAC address: " << macAddress << std::endl;
#else
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <MAC_ADDRESS>" << std::endl;
        return 1;
    }
    const char *macAddress = argv[1];
    std::cout << "Using MAC address: " << macAddress << std::endl;
#endif

    return WolSender(macAddress).run();
}
