//
// Created by liu86 on 25-6-19.
//

#pragma once

#include <string>
#ifdef _WIN32
#include <winsock2.h>
#define SOCKET_FD SOCKET
#else
#define  INVALID_SOCKET -1
#define SOCKET_FD int
#endif
#include <cstdint>

class WolSender {
public:
    explicit WolSender(const std::string& macAddress);

    int run();

private:
    static int initNetwork();
    static void cleanupNetwork();
    static bool parseMacAddress(const std::string& macAddress, uint8_t* macBytes);

    bool openSocket();
    void sendMagicPacket(const uint8_t* macBytes, size_t size) const;
    void closeSocket() const;
private:
    std::string m_macAddress;
    SOCKET_FD m_socket = INVALID_SOCKET;
};
