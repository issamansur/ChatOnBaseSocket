#include <iostream>
#include <string>
#include <vector>
#include <winsock2.h>
#include <ws2tcpip.h>

using namespace std;

int main(int argc, char** argv) {
    // Main Settings
    const char* ip_address = "127.0.0.1"; // is correct without malloc?
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


    // Connecting 
    sockaddr_in service;
    service.sin_family = AF_INET;

    if (inet_pton(AF_INET, ip_address, &service.sin_addr.s_addr) <= 0) {
        cout << "Error in IP address" << endl;
        return 0;
    }
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
    string message_str;

    while (true) {
        printf("Enter your message: ");
        getline(cin, message_str);

        const char* message = message_str.c_str();
        int message_len = strlen(message);
        int chunks_count = (message_len - 1) / (bufferSize - 3) + 1;
        char* current_chunk = (char*)malloc(201 * sizeof(char)); // or 1
        for (int i = 0; i < chunks_count; i++) {
            // zeros chunk
            memset(current_chunk, '\0', bufferSize); // or 0

            // add meta data
            current_chunk[0] = '0' + (i + 1);
            current_chunk[1] = '/';
            current_chunk[2] = '0' + chunks_count;

            // add main data
            memcpy(current_chunk + 3, message, bufferSize - 3);

            // send chunk
            int byteCount = send(clientSocket, current_chunk, bufferSize, 0);
            if (byteCount == SOCKET_ERROR) {
                cout << "Server send error: " << WSAGetLastError() << endl;
                WSACleanup();
                return -1;
            }
            cout << "Client sent chunk ";
            cout << '[' + to_string(i+1) + '/' + to_string(chunks_count) + ']' << endl;

            // increase offset for pointer
            message += bufferSize - 3;
        }
        cout << "Client sent message" << endl;
    }
}
