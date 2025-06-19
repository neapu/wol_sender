//
// Created by liu86 on 25-6-19.
//

#include "WolSender.h"
#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#else
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>
#include <cstring>
#endif
#include <iostream>
#include <cstdint>

WolSender::WolSender(const std::string &macAddress)
    : m_macAddress(macAddress)
{
}

int WolSender::run()
{
    if (const int ret = initNetwork(); ret != 0) {
        return ret; // Handle network initialization error
    }

    std::cout << "Sending magic packet to MAC address: " << m_macAddress << std::endl;

    uint8_t target_mac[6];
    if (!parseMacAddress(m_macAddress, target_mac)) {
        std::cerr << "Invalid MAC address format: " << m_macAddress << std::endl;
        cleanupNetwork();
        return -1; // Handle invalid MAC address error
    }

    std::cout << "Start sending magic packet..." << std::endl;

    if (!openSocket()) {
        cleanupNetwork();
        return -1; // Handle socket opening error
    }

    sendMagicPacket(target_mac, sizeof(target_mac));
    closeSocket();

    std::cout << "Magic packet sent." << std::endl;

    return 0;
}

int WolSender::initNetwork()
{
#ifdef _WIN32
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cout << "WSAStartup failed." << std::endl;
        return -1; // Handle error
    }
#endif
    return 0;
}
void WolSender::cleanupNetwork()
{
#ifdef _WIN32
    WSACleanup();
#endif
}

bool WolSender::parseMacAddress(const std::string& macAddress, uint8_t* macBytes)
{
    // 检查MAC地址长度是否合法 (考虑到分隔符，长度应为17或12)
    if (macAddress.length() != 17 && macAddress.length() != 12) {
        return false;
    }

    // 判断MAC地址格式: xx:xx:xx:xx:xx:xx 或 xx-xx-xx-xx-xx-xx 或 xxxxxxxxxxxx
    bool hasDelimiter = (macAddress.length() == 17);
    char expectedDelimiter = '\0';

    if (hasDelimiter) {
        expectedDelimiter = macAddress[2];
        if (expectedDelimiter != ':' && expectedDelimiter != '-') {
            return false;
        }
    }

    // 解析MAC地址
    int j = 0;
    for (int i = 0; i < 6; ++i) {
        // 读取两个十六进制字符
        std::string byteStr = macAddress.substr(j, 2);
        j += 2;

        // 跳过分隔符
        if (hasDelimiter && i < 5) {
            if (macAddress[j] != expectedDelimiter) {
                return false;
            }
            j++;
        }

        // 将十六进制字符串转换为字节
        char* end;
        long val = strtol(byteStr.c_str(), &end, 16);
        if (*end != '\0' || val < 0 || val > 255) {
            return false;
        }

        macBytes[i] = static_cast<unsigned char>(val);
    }

    return true;
}

bool WolSender::openSocket()
{
    m_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (m_socket == INVALID_SOCKET) {
        std::cerr << "Failed to create socket." << std::endl;
        return false; // Handle error
    }

    // 开启广播选项
#ifdef _WIN32
    BOOL broadcast = TRUE;
    if (setsockopt(m_socket, SOL_SOCKET, SO_BROADCAST, reinterpret_cast<const char*>(&broadcast), sizeof(broadcast)) < 0) {
        std::cerr << "Failed to set socket options." << std::endl;
        return false; // Handle error
    }
#else
    int broadcast = 1;
    if (setsockopt(m_socket, SOL_SOCKET, SO_BROADCAST, &broadcast, sizeof(broadcast)) < 0) {
        std::cerr << "Failed to set socket options." << std::endl;
        return false; // Handle error
    }
#endif
    return true;
}

void WolSender::sendMagicPacket(const uint8_t* macBytes, size_t size) const
{
    // 检查MAC地址大小是否正确
    if (size != 6) {
        std::cerr << "Invalid MAC address size." << std::endl;
        return;
    }

    // 创建魔术包: 6字节的0xFF同步序列 + MAC地址重复16次
    const int packetSize = 6 + 16 * 6; // 6字节同步序列 + MAC地址(6字节)重复16次
    unsigned char packet[packetSize];

    // 填充同步序列
    for (int i = 0; i < 6; ++i) {
        packet[i] = 0xFF;
    }

    // 填充MAC地址16次
    for (int i = 0; i < 16; ++i) {
        for (int j = 0; j < 6; ++j) {
            packet[6 + i * 6 + j] = static_cast<unsigned char>(macBytes[j]);
        }
    }

    // 设置广播地址
    struct sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(9);                            // 标准WOL端口通常是7或9
    addr.sin_addr.s_addr = inet_addr("255.255.255.255"); // 广播地址

    // 发送魔术包
    int sentBytes =
        sendto(m_socket, reinterpret_cast<const char*>(packet), packetSize, 0, reinterpret_cast<struct sockaddr*>(&addr), sizeof(addr));

    if (sentBytes == packetSize) {
        std::cout << "Magic packet sent successfully." << std::endl;
    } else {
        std::cerr << "Failed to send magic packet. Error: ";
#ifdef _WIN32
        std::cerr << WSAGetLastError() << std::endl;
#else
        std::cerr << strerror(errno) << std::endl;
#endif
    }
}

void WolSender::closeSocket() const
{
    if (m_socket != INVALID_SOCKET) {
#ifdef _WIN32
        closesocket(m_socket);
#else
        close(m_socket);
#endif
    }
}
