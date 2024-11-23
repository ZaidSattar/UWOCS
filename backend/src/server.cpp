#include <boost/asio.hpp>
#include <boost/beast/websocket.hpp>
#include <iostream>
#include <string>
#include <thread>
#include "DocumentHandler.h"
#include <nlohmann/json.hpp>

namespace beast = boost::beast;
namespace websocket = beast::websocket;
namespace net = boost::asio;
using tcp = net::ip::tcp;
using json = nlohmann::json;

void handle_session(websocket::stream<tcp::socket> ws) {
    try {
        DocumentHandler docHandler;
        
        // Set suggested timeout settings for the websocket
        ws.set_option(websocket::stream_base::timeout::suggested(
            beast::role_type::server));

        // Set a reasonable read buffer size
        ws.read_message_max(16 * 1024 * 1024); // 16MB

        // Accept the websocket handshake
        ws.accept();
        
        while (true) {
            try {
                beast::flat_buffer buffer;
                ws.read(buffer);
                
                // Handle the message
                std::string message(boost::asio::buffers_begin(buffer.data()),
                                  boost::asio::buffers_end(buffer.data()));
                
                try {
                    auto request = json::parse(message);
                    if (request["type"] == "get_files") {
                        std::string docList = docHandler.listDocuments();
                        ws.text(true);
                        ws.write(net::buffer(docList));
                    }
                    else if (request["type"] == "download") {
                        std::string filename = request["filename"];
                        auto fileData = docHandler.getDocument(filename);
                        ws.binary(true);
                        ws.write(net::buffer(fileData));
                    }
                } catch (const json::parse_error& e) {
                    std::cerr << "JSON parse error: " << e.what() << "\n";
                    continue;
                }
            } catch (const beast::system_error& e) {
                if (e.code() == websocket::error::closed) {
                    std::cout << "WebSocket connection closed normally\n";
                    return;
                }
                throw;
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "Session error: " << e.what() << "\n";
    }
}

int main() {
    try {
        net::io_context ioc;
        tcp::acceptor acceptor{ioc, tcp::endpoint{tcp::v4(), 8080}};
        std::cout << "WebSocket server is running on ws://localhost:8080\n";

        while (true) {
            tcp::socket socket{ioc};
            acceptor.accept(socket);
            std::thread(handle_session, websocket::stream<tcp::socket>(std::move(socket))).detach();
        }
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }
    return 0;
}