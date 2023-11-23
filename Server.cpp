/*
 * Server.cpp
 *
 * Author: Ofek Koren - 207105933, Tomer Aligaev - 208668129
 */

#include "Server.h"

string SocketIO::read() {
    string str = "";
    char c = 'c';
    while (c != '\n') {
        if (recv(this->idClient, &c, sizeof(c), 0) > 0 && c != '\n') {
            str += c;
        }
    }
    return str;
}

void SocketIO::write(string text) {
    send(this->idClient, &text[0], text.size(), 0);

}

void SocketIO::write(float f) {
    std::stringstream stream;
    stream<<f;
    stream.precision(3);
    string floatToString = stream.str();
    write(floatToString);
}

void SocketIO::read(float *f) {
    recv(this->idClient, f, sizeof(*f), 0);
}

void AnomalyDetectionHandler::handle(int clientID) {
    SocketIO socketIo(clientID);
    CLI cli(&socketIo);
    cli.start();
}

Server::Server(int port) throw(const char *) {
    this->shouldRun = true;
    this->fileDescriptor = socket(AF_INET, SOCK_STREAM, 0);
    if (fileDescriptor < 0) {
        close(this->fileDescriptor);
        throw "Couldn't open socket!";
    }
    this->server.sin_family = AF_INET;
    this->server.sin_addr.s_addr = INADDR_ANY;
    this->server.sin_port = htons(port);
    if (bind(fileDescriptor, (struct sockaddr *) &(this->server), sizeof(this->server)) < 0) {
        close(this->fileDescriptor);
        throw "Couldn't bind server";
    }
    if (listen(this->fileDescriptor, SERVER_QUEUE_SIZE) < 0) {
        close(this->fileDescriptor);
        throw "Coulnd't listen";
    }
}

void alarmHandler(int signal) {
    return;
}

void Server::start(ClientHandler &ch) throw(const char *) {
    t = new thread([&ch, this]() {
        signal(SIGALRM, alarmHandler);
        int i=0;
        while (this->shouldRun) {
            i++;
            socklen_t clientStructSize = sizeof(this->client);
            alarm(SIGNAL_TIME);
            int fdClient = accept(this->fileDescriptor, (struct sockaddr *) &(this->client), &clientStructSize);
            if (fdClient < 0) {
                close(fdClient);
                close(this->fileDescriptor);
                throw "Couldn't accept the connection";
            }
            alarm(0);
            ch.handle(fdClient);
            close(fdClient);
        }
        close(this->fileDescriptor);
    });
}

void Server::stop() {
    std::mutex mtx;
    mtx.lock();
    this->shouldRun = false;
    mtx.unlock();
    t->join();
}

Server::~Server() {
    delete this-> t;
}

