#include "CommandHandler.hpp"

#include <iostream>

#include "ProxyServer.hpp"

CommandHandler::CommandHandler(ProxyServer &proxyServer)
    : proxyServer_(proxyServer)
{
    // Register command handlers
    registerCommand("GetUserCount",
                    std::bind(&CommandHandler::handleGetUserConnected, this,
                              std::placeholders::_1, std::placeholders::_2));
    registerCommand("ECHOREPLY",
                    std::bind(&CommandHandler::handleEchoReply, this,
                              std::placeholders::_1, std::placeholders::_2));
    // Add more command handlers as needed
}

void CommandHandler::registerCommand(const std::string &command,
                                     CommandFunction handler)
{
    commandHandlers_[command] = handler;
}

void CommandHandler::handleCommand(
    const std::shared_ptr<boost::asio::ip::tcp::socket> &userSocket,
    const std::string &command)
{
    bool commandHandled = false;

    for (const auto &handler : commandHandlers_)
    {
        const std::string &cmd = handler.first;
        // Check if the command is present in the message
        size_t cmdPos = command.find(cmd);
        if (cmdPos != std::string::npos)
        {
            // Call the corresponding command handler function
            (handler.second)(userSocket, command);
            commandHandled = true;
        }
    }

    if (!commandHandled)
    {
        // If no matching command is found
        std::cout << "Unknown command in message: " << command << std::endl;
    }
}

void CommandHandler::handleGetUserConnected(
    const std::shared_ptr<boost::asio::ip::tcp::socket> &userSocket,
    const std::string &command)
{
    // Handle the "GetUserConnected" command
    proxyServer_.notifyUser(userSocket,
                            "[INFO]UserCount: "
                                + std::to_string(proxyServer_.getUserCount())
                                + "\n");
}

void CommandHandler::handleEchoReply(
    const std::shared_ptr<boost::asio::ip::tcp::socket> &userSocket,
    const std::string &command)
{
    // Find the position of "ECHOREPLY"
    size_t echoPos = command.find("ECHOREPLY");
    if (echoPos != std::string::npos)
    {
        // Extract the rest of the line after "ECHOREPLY"
        std::string restOfLine = command.substr(echoPos + 10); // Assuming "ECHOREPLY" is 9 characters long + the space

        // Notify the user with the rest of the line
        proxyServer_.notifyUser(userSocket, "[CMD]ECHOREPLY: " + restOfLine + "\n");
    }
    else
    {
        std::cout << "Command 'ECHOREPLY' not found in the message: " << command << std::endl;
    }
}
