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
    explicit WolSender(const std::string& macAddress, const std::string& targetIp = "");

    int run();

private:
    static int initNetwork();
    static void cleanupNetwork();
    static bool parseMacAddress(const std::string& macAddress, uint8_t* macBytes);

    bool openSocket();
    void sendMagicPacket(const uint8_t* macBytes, size_t size) const;
    void closeSocket() const;

    // 等待主机上线
    bool waitForHostOnline(const std::string& targetIp, int timeoutSeconds = 60) const;
    bool pingHost(const std::string& targetIp) const;
    bool tryTcpConnect(const std::string& targetIp, int port) const;

private:
    std::string m_macAddress;
    std::string m_targetIp;  // 目标主机IP地址
    SOCKET_FD m_socket = INVALID_SOCKET;
};
