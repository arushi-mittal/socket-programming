# **Networking Assignment**

There are two directories the server and client directory containing a server.c and client.c file respectively.

The socket function is used to create a socket to establish communication between the server and the client.
The TCP protocol is specified in the setsockopt function, and bind is used to bind the port and socket.

The server listens using listen function, waiting for the client to initiate communication. The server stores this accepted socket using accept function in the socket new_socket. Both client and server then try to connect using the connect function. When both are connected successfully, the main program starts.

The client file accepts command line arguments. If the command is exit, the connection is closed and the program exits. If the command is get, the program checks for file names. If no file names are provided, the program exits. If the command is not get or exit, the program exits with the message "Invalid argument".

After checking for filenames, the client sends the name of a file to the server. The server checks if the file is a regular file using struct stat. and S_ISREG. If it's not a regular file, size of -1 is sent. The server checks if the file exists using a file pointer. If the file exists, the server shares the size of the file with the client. If not, the server shares -1. If the client receives -1, it responds by sending the next filename. Otherwise it begins to receive the file. The server reads the file into a buffer of size 1024 bytes (this can be changed in the #define bs 1024 in both codes). If the file is smaller than the buffer it is sent in a single transmission, otherwise it is sent in batches of 1024. Each time a batch is sent, the buffer size is divided by the total size and displayed as a percentage. The carriage return character is used to show the progress of the file transfer. Similarly, the client receives 1024 bytes and displays them as a percentage of the total size. 

The bzero function is used to set the buffer to an empty state and file pointers and fseek are used to maintain the position of the pointer during traversal of the file. 

An important thing to note is that in case a file already exists in the client and requests download from the server, the previous file is overwritten with the contents of the new one. Additionally, empty files can be transferred. The basic assumption on which this code works is that client and server are both in separate directories.

Standard file handling procedures and libraries are used in order to ensure the program runs smoothly in addition to socket programming and networking libraries.


## **Instructions to run**:

Open two terminals, one for the client and one for the server.

First Terminal:
  
```bash
$ cd socket-programming/server
$ gcc server.c
```

Second Terminal:
  
```bash
$ cd socket-programming/client
$ gcc client.c
```

First Terminal:
  
```bash
./a.out
```

Second Terminal:
  
```bash
./a.out <command name> <args>
```

Depending on the verification of the commands, output will appear on both terminals. While server will display notifications when it receives a request, the progress, and when the transfer is finished, client will display notifications when it receives the file size (or the file does not exist), the progress and when the transfer is completed.
