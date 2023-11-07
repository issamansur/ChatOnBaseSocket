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
- In main view select `Additional Dependencies` and `Edit...`
- In opened subwindow in input print `ws2_32.lib` and click `OK`
- Click `Apply` and then `OK`
- Done!

## Tasks

### Main
- [x] Connect `Client` with `Server`
- [x] Add args for cli (Command line)
- [x] `Client` can send message with size more than `bufferSize`
- [x] `Client` can send more than one message
- [ ] `Server` can respond on `Client`'s message

- [ ] `Server` can accept and listen more than one `Client`
- [ ] `Server` can receive message from one `Client` and MAYBE send this message to second `Client` 


### Additional
- [x] Change splitting: vector<string> -> char* (for optimize)
- [ ] Add formatted output (BETA)

## Example

### Server
```
IP ADDRESS: 127.0.0.1
PORT: 55555

The Winsock dll found!
The status: Running
socket() is OK!
bind() is OK!
Start listening on port: 55555
Accepted connection
Server received header
Server received chunk [1/1]
[2023-10-27 21:50:19] issamansur: Hi
```

### Client
```
IP ADDRESS: 127.0.0.1
PORT: 55555

The Winsock dll found!
The status: Running
socket() is OK!
Client connect() is OK!
Enter your username (5-20 symbols): issamansur
issamansur (YOU) > Hi
Client sent header
Client sent chunk [1/1]
Client sent message
issamansur (YOU) > _
```

### Interesting links for updates

- [Formatting output](https://ru.stackoverflow.com/questions/1243530/%D0%9A%D0%B0%D0%BA-%D0%BE%D0%B4%D0%BD%D0%BE%D0%B2%D1%80%D0%B5%D0%BC%D0%B5%D0%BD%D0%BD%D0%BE-%D0%B2%D0%B2%D0%BE%D0%B4%D0%B8%D1%82%D1%8C-%D0%B8-%D0%B2%D1%8B%D0%B2%D0%BE%D0%B4%D0%B8%D1%82%D1%8C-%D0%B4%D0%B0%D0%BD%D0%BD%D1%8B%D0%B5-%D0%B2-%D0%BA%D0%BE%D0%BD%D1%81%D0%BE%D0%BB%D1%8C-python?ysclid=lomiqw8bly490441757)
