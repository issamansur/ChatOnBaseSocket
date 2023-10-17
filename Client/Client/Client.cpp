#include <iostream>
#include <string>
#include <vector>
#include <winsock2.h>
#include <ws2tcpip.h>

using namespace std;


vector<string> splitString(const string& str, int n) {
    vector<string> result;
    int length = str.length();

    for (int i = 0; i < length; i += n) {
        string chunk = str.substr(i, n);
        if (chunk.length() < n) {
            chunk += string(n - chunk.length(), '\0');
        }
        result.push_back(chunk);
    }

    return result;
}

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
    SOCKET clientSocket;

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

    // Send data
    const int bufferSize = 200;
    string message;

    while (true) {
        printf("Enter your message: ");
        getline(cin, message);
        vector<string> chunks = splitString(message, bufferSize - 4);

        int i = 0;
        int n = chunks.size();

        for (string chunk : chunks) {
            // add meta data
            string new_chunk = to_string(++i) + '/' + to_string(n);
            //add main data
            new_chunk += chunk;

            // send
            int byteCount = send(clientSocket, new_chunk.c_str(), bufferSize, 0);
            if (byteCount == SOCKET_ERROR) {
                cout << "Server send error: " << WSAGetLastError() << endl;
                WSACleanup();
                return -1;
            }
            cout << "Client sent chunk ";
            cout << '[' + to_string(i) + '/' + to_string(n) + ']' << endl;
        }
        cout << "Client sent message" << endl;
    }
}
