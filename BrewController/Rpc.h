#pragma once
#include <string>
#include <thread>
#include <atomic>
#include <json/json.h>

class Rpc {
public:
    Rpc(int port);
    ~Rpc();

    void start();
    void stop();

private:
    int port;
    int serverSocket;
    std::atomic<bool> running;
    std::thread serverThread;

    void runServer();
    void handleClient(int clientSocket);
    std::string processCommand(const std::string &jsonInput);
    bool validateCommand(const Json::Value &command, std::string &error);

    // API functions
    Json::Value getDebugDump();
    Json::Value getTemperature();
    Json::Value getPressure();
    Json::Value controlHeater(bool on);
    Json::Value controlPump(int seconds);
};