
#include <iostream>
#include <sstream>
#include <thread>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <atomic>
#include "Rpc.h"
Rpc::Rpc(int port) : port(port), serverSocket(-1), running(false) {}

Rpc::~Rpc() {
    stop();
    if (serverSocket >= 0) {
        close(serverSocket);
    }
}

void Rpc::start() {
    running = true;
    serverThread = std::thread(&Rpc::runServer, this);
}

void Rpc::stop() {
    running = false;
    if (serverThread.joinable()) {
        serverThread.join();
    }
}

void Rpc::runServer() {
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket < 0) {
        throw std::runtime_error("Error creating socket.");
    }

    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(port);

    if (bind(serverSocket, (sockaddr *)&serverAddr, sizeof(serverAddr)) < 0) {
        throw std::runtime_error("Error binding to port.");
    }

    if (listen(serverSocket, 5) < 0) {
        throw std::runtime_error("Error listening on socket.");
    }

    std::cout << "Server listening on port " << port << "..." << std::endl;

    while (running) {
        sockaddr_in clientAddr;
        socklen_t clientLen = sizeof(clientAddr);
        int clientSocket = accept(serverSocket, (sockaddr *)&clientAddr, &clientLen);
        if (clientSocket < 0) {
            if (running) {
                std::cerr << "Error accepting connection." << std::endl;
            }
            continue;
        }

        std::thread(&Rpc::handleClient, this, clientSocket).detach();
    }

    close(serverSocket);
    serverSocket = -1;
}

void Rpc::handleClient(int clientSocket) {
  int ret=0;
    char buffer[1024];
    memset(buffer, 0, sizeof(buffer));

    int bytesRead = read(clientSocket, buffer, sizeof(buffer) - 1);
    if (bytesRead < 0) {
        std::cerr << "Error reading from client socket." << std::endl;
        close(clientSocket);
        return;
    }

    std::string request(buffer);
    std::string response = processCommand(request);

    ret=write(clientSocket, response.c_str(), response.size());
    if(0 != ret)
    {
      std::cerr << "Error writting from client socket." << std::endl;

    }
    ret=close(clientSocket);
    if(0 != ret)
    {
      std::cerr << "Error closing client socket." << std::endl;

    }
}

bool Rpc::validateCommand(const Json::Value &command, std::string &error) {
    if (!command.isMember("command") || !command["command"].isString()) {
        error = "Missing or invalid 'command'.";
        return false;
    }

    if (!command.isMember("command_id") || !command["command_id"].isInt()) {
        error = "Missing or invalid 'command_id'.";
        return false;
    }

    return true;
}

std::string Rpc::processCommand(const std::string &jsonInput) {
    Json::Value root;
    Json::CharReaderBuilder readerBuilder;
    std::string errs;

    std::istringstream iss(jsonInput);
    if (!Json::parseFromStream(readerBuilder, iss, &root, &errs)) {
        return R"({"command": "error", "command_id": -1, "response": ["Invalid JSON"]})";
    }

    std::string error;
    if (!validateCommand(root, error)) {
        Json::Value response;
        response["command"] = "error";
        response["command_id"] = root.isMember("command_id") ? root["command_id"].asInt() : -1;
        response["response"].append(error);
        Json::StreamWriterBuilder writer;
        return Json::writeString(writer, response);
    }

    std::string command = root["command"].asString();
    int commandId = root["command_id"].asInt();

    Json::Value response;
    response["command_id"] = commandId;
    response["command"] = command;

    if (command == "get_debug_dump") {
        response["response"] = getDebugDump();
    } else if (command == "get_temperature") {
        response["response"] = getTemperature();
    } else if (command == "get_pressure") {
        response["response"] = getPressure();
    } else if (command == "control_heater") {
        if (!root.isMember("parameters") || !root["parameters"].isMember("state")) {
            response["response"] = R"(["Missing 'state' parameter"]])";
        } else {
            bool state = root["parameters"]["state"].asBool();
            response["response"] = controlHeater(state);
        }
    } else if (command == "control_pump") {
        if (!root.isMember("parameters") || !root["parameters"].isMember("duration")) {
            response["response"] = R"(["Missing 'duration' parameter"]])";
        } else {
            int duration = root["parameters"]["duration"].asInt();
            response["response"] = controlPump(duration);
        }
    } else {
        response["response"] = R"(["Unknown command"]])";
    }

    Json::StreamWriterBuilder writer;
    return Json::writeString(writer, response);
}

Json::Value Rpc::getDebugDump() {
    Json::Value dump;
    dump["temperature"] = 25.0;
    dump["pressure"] = 101.3;
    dump["heater_state"] = false;
    dump["pump_status"] = "idle";
    return dump;
}

Json::Value Rpc::getTemperature() {
    Json::Value temp;
    temp["temperature"] = 25.0; // Example value
    return temp;
}

Json::Value Rpc::getPressure() {
    Json::Value pressure;
    pressure["pressure"] = 101.3; // Example value
    return pressure;
}

Json::Value Rpc::controlHeater(bool on) {
    Json::Value response;
    response["heater_state"] = on ? "on" : "off";
    return response;
}

Json::Value Rpc::controlPump(int seconds) {
    Json::Value response;
    if (seconds <= 0) {
        response["error"] = "Invalid duration.";
    } else {
        response["pump_status"] = "running";
        response["duration"] = seconds;
    }
    return response;
}
