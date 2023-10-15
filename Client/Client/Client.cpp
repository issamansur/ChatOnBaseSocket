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
    SOCKET clientSocket, acceptSocket;

    clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (clientSocket == INVALID_SOCKET) {
        cout << "Error at socket(): " << WSAGetLastError() << endl;
        WSACleanup();
        return 0;
    }
    else {
        cout << "socket() is OK!" << endl;
    }


    // Connecting client
    int port = 55555;

    sockaddr_in service;
    service.sin_family = AF_INET;
    InetPton(AF_INET, L"127.0.0.1", &service.sin_addr.s_addr);
    service.sin_port = htons(port);
    if (connect(clientSocket, (SOCKADDR*)&service, sizeof(service)) == SOCKET_ERROR) {
        cout << "Connecting failed: " << WSAGetLastError() << endl;
        closesocket(clientSocket);
        WSACleanup();
        return -1;
    }
    cout << "Client connect() is OK!" << endl;

    // Send message
    char buffer[200];
    printf("Enter your messsage: ");
    cin.getline(buffer, 200);
    int byteCount = send(clientSocket, buffer, 200, 0);
    if (byteCount == SOCKET_ERROR) {
        cout << "Server send error: " << WSAGetLastError() << endl;
        WSACleanup();
        return -1;
    }

    cout << "Server sent message" << endl;
}