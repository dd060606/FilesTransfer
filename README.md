# FilesTransfer
FilesTransfer allows you to securely transfer files within your company or with your friends from the command prompt.
This program is written in C++ with Qt.

## Server
Start the server:

```
FilesTransferServer.exe <port>
```
The default port is 55630

#### Commands:
```
help - Shows the list of commands
list - Shows all clients and their id
send <clientId> <"hostPath"> <"clientOutputPath"> - Send a file to a client
get <clientId> <"clientPath"> <"hostOutputPath"> - Receive a file from a client
disconnect <clientId> - Remotely disconnect a client
```

## Client
```
FilesTransferClient.exe <host> <port> startup
```
The startup argument is optional. If you add it, FilesTransfer will start automatically at computer startup.