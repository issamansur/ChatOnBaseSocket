# ChatOnBaseSocket

## The principle of operation:
1. Initialize WSA - `WSAStartup()`
2. Create a socket - `socket()`
3. Bind the socket - `bind()`
4. Listen on the socket - `listen()`
5. Accept a connection - `accept()`/`connect()`
6. Send and receive data - `recv()`, `recvfrom`/`send()`, `sendto()`
7. Disconnect - `closesocket()`

## From abstraction to realization:

### Preparing
Before you develop Windows Sockets programs within Visual Studio, you must include the `ws2_32.lib` library within your Project.

Create a new `C++ Console Project` and follow the steps belows:
- Select the `Project` menu
- Select the `Properties` (by default `Alt+F7`)
- In opened window in left menu select `Linker`
- In submenu select `Input`
- In main view select `Additional Dependencies` and `Change...`
- In opened subwindow in input print `ws2_32.lib` and click `OK`
- Click `Apply` and then `OK`
- Done!

## Tasks
- [x] - Connect `Client` with `Server`
- [ ] - `Client` can send more than one message
- [ ] - `Server` can accept and listen more than one `Client`
- [ ] - `Server` can receive message from one `Client` and MAYBE send this message to second `Client` 
