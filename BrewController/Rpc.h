#ifndef RPC_H
#define RPC_H

#include <string>
#include <json/json.h>

class RPC {
public:
    RPC(int port);
    ~RPC();

    void start();

private:
    int port;
    int serverSocket;

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

#endif // RPC_H
