#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>

using namespace std;

int main() {
    WSADATA wsaData;
    int wsa_startup_return_code;
    WORD wVersion = MAKEWORD(2, 2);

    wsa_startup_return_code = WSAStartup(wVersion, &wsaData);
    if (wsa_startup_return_code != 0) {
        cout << "WSAStartup failed with error code: " << wsa_startup_return_code << endl;
        return 1;
    }

    SOCKET serverSocket, acceptSocket;
    serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (serverSocket == INVALID_SOCKET) {
        cout << "Error at socket(): " << WSAGetLastError() << endl;
        WSACleanup();
        return 1;
    }

    sockaddr_in service;
    int port = 55555;
    service.sin_family = AF_INET;
    InetPton(AF_INET, L"127.0.0.1", &service.sin_addr.s_addr);
    service.sin_port = htons(port);

    if (bind(serverSocket, (SOCKADDR*)&service, sizeof(service)) == SOCKET_ERROR) {
        cout << "Binding failed: " << WSAGetLastError() << endl;
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    if (listen(serverSocket, 1) == SOCKET_ERROR) {
        cout << "Error listening on socket: " << WSAGetLastError() << endl;
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    cout << "Start listening on port: " << port << endl;

    acceptSocket = accept(serverSocket, NULL, NULL);
    if (acceptSocket == INVALID_SOCKET) {
        cout << "Accept failed: " << WSAGetLastError() << endl;
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    cout << "Accepted connection" << endl;

    char receiveBuffer[200] = "";
    int byteCount = recv(acceptSocket, receiveBuffer, 200, 0);
    if (byteCount < 0) {
        cout << "Server error: " << WSAGetLastError() << endl;
    } else {
        cout << "Received data: " << receiveBuffer << endl;
    }

    // Properly close the sockets and clean up Winsock
    closesocket(acceptSocket);
    closesocket(serverSocket);
    WSACleanup();

    return 0;
}
