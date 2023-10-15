#include <iostream>

#include <winsock2.h>
#include <ws2tcpip.h>

using namespace std;

int main()
{
    // Initialize WSA
    WSADATA wsaData;
    int wsa_startup_return_code;
    WORD wVersion = MAKEWORD(2, 2);

    wsa_startup_return_code = WSAStartup(wVersion, &wsaData);
    if (wsa_startup_return_code == 0) {
        cout << "The Winsock dll found!" << endl;
        cout << "The status: " << wsaData.szSystemStatus << endl;
    }
    else {
        cout << "The Winsock dll found!" << endl;
        return 0;
    }


    // Create a socket
    SOCKET serverSocket, acceptSocket;

    serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (serverSocket == INVALID_SOCKET) {
        cout << "Error at socket(): " << WSAGetLastError() << endl;
        WSACleanup();
        return 0;
    }
    else {
        cout << "socket() is OK!" << endl;
    }


    // Bind the socket with port
    int port = 55555;
    sockaddr_in service;
    service.sin_family = AF_INET;
    
    InetPton(AF_INET, L"127.0.0.1", &service.sin_addr.s_addr);
    service.sin_port = htons(port);
    if (bind(serverSocket, (SOCKADDR*)&service, sizeof(service)) == SOCKET_ERROR) {
        cout << "Binding failed: " << WSAGetLastError() << endl;
        closesocket(serverSocket);
        WSACleanup();
        return 0;
    }
    cout << "bind() is OK!" << endl;

    // Listen on the socket
    if (listen(serverSocket, 1) == SOCKET_ERROR) {
        cout << "Error listening on socket: " << WSAGetLastError() << endl;
        return 0;
    }
    else {
        cout << "Start listening on port: " << port << endl;
    }

    // Accept a connection
    acceptSocket = accept(serverSocket, NULL, NULL);
    if (acceptSocket == INVALID_SOCKET) {
        cout << "Accept failed: " << WSAGetLastError() << endl;
        WSACleanup();
        return -1;
    }
    else {
        cout << "Accepted connection" << endl;
        //system("pause");
        //WSACleanup();
    }

    // Receive data
    char receiveBuffer[200] = "";
    int byteCount = recv(acceptSocket, receiveBuffer, 200, 0);
    if (byteCount < 0) {
        cout << "Server error!" << WSAGetLastError() << endl;
        return -1;
    }

    cout << receiveBuffer;
}
