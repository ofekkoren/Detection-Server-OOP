/*
 * Server.h
 *
 * Author: Ofek Koren - 207105933, Tomer Aligaev - 208668129
 */

#ifndef SERVER_H_
#define SERVER_H_

#include "commands.h"
#include <sys/socket.h>
#include "CLI.h"
#include <pthread.h>
#include <thread>
#include <netinet/in.h>
#include <unistd.h>
#include <iostream>
#include <signal.h>
#include <atomic>
#include <mutex>

#define SERVER_QUEUE_SIZE 2
#define SIGNAL_TIME 3

using namespace std;

// edit your ClientHandler interface here:
class ClientHandler {
public:
    virtual void handle(int clientID) = 0;
};

// you can add helper classes here and implement on the cpp file
class SocketIO : public DefaultIO {
    int idClient;
public:
    SocketIO(int idClient) {
        this->idClient = idClient;
    }

    virtual string read();

    virtual void write(string text);

    virtual void write(float f);

    virtual void read(float *f);
};


// edit your AnomalyDetectionHandler class here
class AnomalyDetectionHandler : public ClientHandler {
public:
    virtual void handle(int clientID);
};


// implement on Server.cpp
class Server {
    thread *t; // the thread to run the start() method in
    int fileDescriptor;
    sockaddr_in server;
    sockaddr_in client;
    atomic<bool> shouldRun;

    void startHelper(ClientHandler &ch) throw(const char *);

    // you may add data members

public:
    Server(int port) throw(const char *);

    virtual ~Server();

    void start(ClientHandler &ch) throw(const char *);

    void stop();
};

#endif /* SERVER_H_ */
