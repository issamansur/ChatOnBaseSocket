# ChatOnBaseSocket

## The principle of operation:
1. Initialize WSA - `WSAStartup()`
2. Create a socket - `socket()`
3. Bind the socket - `bind()`
4. Listen on the socket - `listen()`
5. Accept a connection - `accept()`/`connect()`
6. Send and receive data - `recv()`, `recvfrom`/`send()`, `sendto()`
7. Disconnect - `closesocket()`
