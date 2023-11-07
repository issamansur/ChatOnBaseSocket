#include <iostream>
#include <string>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <thread>
#include <vector>
#include <mutex>

using namespace std;

// Global vars
int currentClientCount = 0;
mutex clientCountMutex;

const int maxClients = 10;

vector<SOCKET> clientSockets;
mutex clientSocketsMutex;

// Functions
void clientHandler(SOCKET clientSocket);
void respond(SOCKET acceptSocket, string message);
void on_error(int wsaCode);

int main(int argc, char** argv) {
    // Main Settings
    const char* ip_address = "127.0.0.1";
    int port = 55555;

    // Handler of command line arguments
    for (int i = 2; i < argc; i += 2) {
        if (strcmp(argv[i - 1], "-ip") == 0)
            ip_address = argv[i];
        else if (strcmp(argv[i - 1], "-port") == 0)
            port = atoi(argv[i]);
    }

    cout << "IP ADDRESS: " << ip_address << endl;
    cout << "PORT: " << port << endl << endl;

    // Initialize WSA
    WSADATA wsaData;
    WORD wVersion = MAKEWORD(2, 2);

    if (WSAStartup(wVersion, &wsaData) == 0) {
        cout << "The Winsock dll found!" << endl;
        cout << "The status: " << wsaData.szSystemStatus << endl;
    }
    else {
        cout << "The Winsock dll not found!" << endl;
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
    cout << "socket() is OK!" << endl;


    // Bind the socket with port
    sockaddr_in service;
    service.sin_family = AF_INET;

    if (inet_pton(AF_INET, ip_address, &service.sin_addr.s_addr) <= 0) {
        cout << "Error in IP address" << endl;
        return 0;
    }

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
        closesocket(serverSocket);
        WSACleanup();
        return 0;
    }
    cout << "Start listening on port: " << port << endl;


    // Accept a connections

    while (true) {
        acceptSocket = accept(serverSocket, NULL, NULL);
        if (acceptSocket == INVALID_SOCKET) {
            cout << "Accept failed: " << WSAGetLastError() << endl;
            closesocket(acceptSocket);
            WSACleanup();
            continue;
        }
		else
        {
            lock_guard<mutex> lock(clientCountMutex);
            if (currentClientCount < maxClients) {
                currentClientCount++;

                cout << "Accepted connection";
                cout << ' ' << currentClientCount << '/' << maxClients << endl;

                {
                    lock_guard<mutex> lock(clientSocketsMutex);
                    clientSockets.push_back(acceptSocket);
                }

                thread clientThread(clientHandler, acceptSocket);
                clientThread.detach();
            }
            else {
                cout << "Maximum client limit reached. Connection refused." << endl;
                closesocket(acceptSocket);
            }
        }
    }

    // Properly close the socket and clean up Winsock
    closesocket(serverSocket);
    WSACleanup();

    return 0;
}

void clientHandler(SOCKET clientSocket) {
    const int bufferSize = 200;
    char receiveBuffer[bufferSize + 1] = "";

    int byteCount = 0;
    while (byteCount >= 0) {
        string message = "";
        int byteCount = recv(clientSocket, receiveBuffer, bufferSize, 0);

        if (byteCount < 0) {
            on_error(WSAGetLastError());
            break;
        }

        if (receiveBuffer[1] != '/' && receiveBuffer[0] != '0') {
            cout << "Server received broken package. Continue receive..." << endl;
        }
        else {
            // cout << "Server received header" << endl;
            message += string(receiveBuffer + 3);
            message += string(": ");
        }

        while (byteCount >= 0) {
            int byteCount = recv(clientSocket, receiveBuffer, bufferSize, 0);
            if (receiveBuffer[1] != '/') {
                cout << "Server received broken package. Continue receive..." << endl;
            }
            else {
                int i = int(receiveBuffer[0] - '0');
                int n = int(receiveBuffer[2] - '0');
                /*
                cout << "Server received chunk ";
                cout << '[' + to_string(i) + '/' + to_string(n) + ']' << endl;
                */
                message += string(receiveBuffer + 3);

                if (i == n)
                    break;
            }
        }
        if (byteCount < 0) {
            on_error(WSAGetLastError());
            break;
        }
        else {
            cout << message << endl;

            respond(clientSocket, "[Server]: Message received!");
        }
    }

    // Close socket and remove from `clientSockets`
    closesocket(clientSocket);
    {
        lock_guard<mutex> lock(clientSocketsMutex);
        lock_guard<mutex> lock2(clientCountMutex);
        currentClientCount--;
        clientSockets.erase(remove(clientSockets.begin(), clientSockets.end(), clientSocket), clientSockets.end());
    }
    cout << "Client's socket was closed";
    cout << ' ' << currentClientCount << '/' << maxClients << endl;
}


void respond(SOCKET acceptSocket, string message) {
    int byteCount = send(acceptSocket, message.c_str(), message.length(), 0);
    if (byteCount == SOCKET_ERROR) {
        cout << "Server send error: " << WSAGetLastError() << endl;
    }
}

void on_error(int wsaCode) {
    if (wsaCode == 10054)
		cout << "Client has terminated the connection" << endl;
	else
		cout << "Server error: " << wsaCode << endl;
}