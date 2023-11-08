#include <iostream>
#include <chrono>
#include <ctime>
#include <string>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <thread>
#include <mutex>
#include <windows.h>

using namespace std;
short curPosOutputY = 0;
mutex cursor;

void receiveMessages(SOCKET clientSocket);
COORD getCursorPosition();
void setCursorPosition(int x, int y);

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

    // Input username
    string username_str;
    const char* username = "";
    int username_len = 0;

    while (username_len < 5 or username_len > 20) {
        printf("Enter your username (5-20 symbols): ");
        getline(cin, username_str);
        username = username_str.c_str();
        username_len = strlen(username);
    }

    thread receiveThread(receiveMessages, clientSocket);
    receiveThread.detach();

    curPosOutputY = getCursorPosition().Y;

    // Send data
    const int bufferSize = 200;
    const int metaSize = 3;
    string message_str;

    while (true) {
        {
            lock_guard<mutex> lock(cursor);

            setCursorPosition(0, curPosOutputY);
            for (int i = 0; i < 8192; i++)
                cout << ' ';
            setCursorPosition(0, max(curPosOutputY - 12, 0));
            setCursorPosition(0, curPosOutputY + 17);

            printf(username);
            printf(" (YOU) > ");
        }
        // input message
        getline(cin, message_str);
        const char* message = message_str.c_str();
        
        // commands handler
        if (strcmp(message, "/exit") == 0) {
			cout << "Client closed the connection." << endl;
			break;
		}


        int message_len = strlen(message);
        int chunks_count = (message_len - 1) / (bufferSize - metaSize) + 1;
        char* current_chunk = (char*)malloc(bufferSize * sizeof(char)); // or 1

        // send header
        if (current_chunk != nullptr) {
            // zeros chunk
            memset(current_chunk, '\0', bufferSize); // or 0

            // add meta data
            current_chunk[0] = '0';
            current_chunk[1] = '/';
            current_chunk[2] = '0' + chunks_count;

            // add main data
            // add date
            time_t t = time(0);   // get time now
            tm* now = (tm*)malloc(sizeof(tm));
            localtime_s(now, &t);
            char* timeString = (char*)malloc(33);
            sprintf_s(timeString, 33, "[%04d-%02d-%02d %02d:%02d:%02d] ",
                now->tm_year + 1900, now->tm_mon + 1, now->tm_mday,
                now->tm_hour, now->tm_min, now->tm_sec);
            memcpy(current_chunk + metaSize, timeString, strlen(timeString));
            // add username
            memcpy(current_chunk + metaSize + strlen(timeString), username, username_len);

            int byteCount = send(clientSocket, current_chunk, bufferSize, 0);
            if (byteCount == SOCKET_ERROR) {
                cout << "Server send error: " << WSAGetLastError() << endl;
                WSACleanup();
                return -1;
            }
            // cout << "Client sent header" << endl;

            free(now);
            free(timeString);
        }

        // send message by chunks
        for (int i = 0; i < chunks_count; i++) {
            if (current_chunk != nullptr) {
                // zeros chunk
                memset(current_chunk, '\0', bufferSize); // or 0

                // add meta data
                current_chunk[0] = '0' + (i + 1);
                current_chunk[1] = '/';
                current_chunk[2] = '0' + chunks_count;

                // add main data
                memcpy(current_chunk + metaSize, message, bufferSize - metaSize);

                // send chunk
                int byteCount = send(clientSocket, current_chunk, bufferSize, 0);
                if (byteCount == SOCKET_ERROR) {
                    cout << "Server send error: " << WSAGetLastError() << endl;
                    WSACleanup();
                    return -1;
                }
                /*
                cout << "Client sent chunk ";
                cout << '[' + to_string(i + 1) + '/' + to_string(chunks_count) + ']' << endl;
				*/

                // increase offset for pointer
                message += bufferSize - metaSize;
            }
            else
                cout << "Error with memory" << endl;
        }
        // cout << "Client sent message" << endl;

        free(current_chunk);
        /*
        // Receive respond from server
        char responseBuffer[bufferSize + 1] = "";
        int responseByteCount = recv(clientSocket, responseBuffer, bufferSize, 0);
        if (responseByteCount == SOCKET_ERROR) {
            cout << "Client receive error: " << WSAGetLastError() << endl;
        }
        else {
            cout << responseBuffer << endl;
        }
        */
    }
}

void receiveMessages(SOCKET clientSocket) {
    const int bufferSize = 200;

    while (true) {
        char receiveBuffer[bufferSize + 1] = "";
        int byteCount = recv(clientSocket, receiveBuffer, bufferSize, 0);
        
        {
            lock_guard<mutex> lock(cursor);

            COORD curPos = getCursorPosition();
            setCursorPosition(0, curPosOutputY);
            if (byteCount == SOCKET_ERROR) {
                cout << "Client receive error: " << WSAGetLastError() << endl;
                break;
            }
            else if (byteCount == 0) {
                cout << "Server closed the connection." << endl;
                break;
            }
            else {
                cout << receiveBuffer << endl;
            }
            curPosOutputY = getCursorPosition().Y;
            setCursorPosition(curPos.X, curPos.Y);
        }
    }
}

COORD getCursorPosition() {
    CONSOLE_SCREEN_BUFFER_INFO csbi;
	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
	return csbi.dwCursorPosition;
}

void setCursorPosition(int x, int y) {
	static const HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
	cout.flush();
	COORD coord = { (SHORT)x, (SHORT)y };
	SetConsoleCursorPosition(hOut, coord);
}