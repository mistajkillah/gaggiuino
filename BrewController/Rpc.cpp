#include "RPC.h"
#include <iostream>
#include <sstream>
#include <thread>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <atomic>

RPC::RPC(int port) : port(port), serverSocket(-1), running(false) {}

RPC::~RPC() {
    stop();
    if (serverSocket >= 0) {
        close(serverSocket);
    }
}

void RPC::start() {
    running = true;
    serverThread = std::thread(&RPC::runServer, this);
}

void RPC::stop() {
    running = false;
    if (serverThread.joinable()) {
        serverThread.join();
    }
}

void RPC::runServer() {
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

        std::thread(&RPC::handleClient, this, clientSocket).detach();
    }

    close(serverSocket);
    serverSocket = -1;
}

void RPC::handleClient(int clientSocket) {
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

    write(clientSocket, response.c_str(), response.size());
    close(clientSocket);
}

bool RPC::validateCommand(const Json::Value &command, std::string &error) {
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

std::string RPC::processCommand(const std::string &jsonInput) {
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

Json::Value RPC::getDebugDump() {
    Json::Value dump;
    dump["temperature"] = 25.0;
    dump["pressure"] = 101.3;
    dump["heater_state"] = false;
    dump["pump_status"] = "idle";
    return dump;
}

Json::Value RPC::getTemperature() {
    Json::Value temp;
    temp["temperature"] = 25.0; // Example value
    return temp;
}

Json::Value RPC::getPressure() {
    Json::Value pressure;
    pressure["pressure"] = 101.3; // Example value
    return pressure;
}

Json::Value RPC::controlHeater(bool on) {
    Json::Value response;
    response["heater_state"] = on ? "on" : "off";
    return response;
}

Json::Value RPC::controlPump(int seconds) {
    Json::Value response;
    if (seconds <= 0) {
        response["error"] = "Invalid duration.";
    } else {
        response["pump_status"] = "running";
        response["duration"] = seconds;
    }
    return response;
}
