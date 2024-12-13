#include <boost/asio.hpp>
#include <boost/beast/websocket.hpp>
#include <iostream>
#include <string>
#include <thread>
#include "DocumentHandler.h"
#include <nlohmann/json.hpp>
#include "ScheduleHandler.h"
#include "../include/GPACalculator.h"
#include "../include/GraduationProgress.h"
#include "../include/CoursePlanner.h"

namespace beast = boost::beast;
namespace websocket = beast::websocket;
namespace net = boost::asio;
using tcp = net::ip::tcp;
using json = nlohmann::json;

void handle_session(websocket::stream<tcp::socket> ws) {
    try {
        DocumentHandler docHandler;
        ScheduleHandler scheduleHandler;
        
        // Set suggested timeout settings for the websocket
        ws.set_option(websocket::stream_base::timeout::suggested(
            beast::role_type::server));

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
                
                std::cout << "Debug: Received WebSocket message: " << message << std::endl;
                
                try {
                    auto request = json::parse(message);
                    if (request["type"] == "get_files") {
                        std::string docList = docHandler.listDocuments();
                        ws.text(true);
                        ws.write(net::buffer(docList));
                    }
                    else if (request["type"] == "download") {
                        try {
                            std::string filename = request["filename"];
                            
                            // Send a start message
                            json startResponse = {
                                {"type", "downloadStart"},
                                {"filename", filename}
                            };
                            ws.text(true);
                            ws.write(net::buffer(startResponse.dump()));

                            // Get and send the file data
                            auto fileData = docHandler.getDocument(filename);
                            if (fileData.empty()) {
                                json errorResponse = {
                                    {"type", "downloadError"},
                                    {"message", "File not found or empty"}
                                };
                                ws.text(true);
                                ws.write(net::buffer(errorResponse.dump()));
                            } else {
                                // Send the actual file data
                                ws.binary(true);
                                ws.write(net::buffer(fileData));

                                // Send completion message
                                json completeResponse = {
                                    {"type", "downloadComplete"},
                                    {"filename", filename}
                                };
                                ws.text(true);
                                ws.write(net::buffer(completeResponse.dump()));
                            }
                        } catch (const std::exception& e) {
                            json errorResponse = {
                                {"type", "downloadError"},
                                {"message", std::string("Error downloading file: ") + e.what()}
                            };
                            ws.text(true);
                            ws.write(net::buffer(errorResponse.dump()));
                        }
                    }
                    else if (request["type"] == "getSchedule") {
                        std::string schedule = scheduleHandler.getSchedule();
                        ws.text(true);
                        ws.write(net::buffer(schedule));
                    }
                    else if (request["type"] == "addScheduleEvent") {
                        bool success = scheduleHandler.addEvent(request["event"]);
                        json response = {
                            {"type", "scheduleResponse"},
                            {"success", success}
                        };
                        ws.text(true);
                        ws.write(net::buffer(response.dump()));
                    }
                    else if (request["type"] == "calculateGPA") {
                        GPACalculator calculator;
                        std::string response = calculator.calculateGPA(message);
                        std::cout << "GPA Response: " << response << std::endl;  // Debug output
                        ws.text(true);
                        ws.write(net::buffer(response));
                    }
                    else if (request["type"] == "getModuleRequirements") {
                        GraduationProgress progress;
                        std::string response = progress.getModuleRequirements(request["moduleId"]);
                        ws.text(true);
                        ws.write(net::buffer(response));
                    }
                    else if (request["type"] == "calculateProgress") {
                        GraduationProgress progress;
                        std::vector<std::string> completedCourses;
                        for (const auto& course : request["completedCourses"]) {
                            completedCourses.push_back(course);
                        }
                        std::string response = progress.calculateProgress(request["moduleId"], completedCourses);
                        ws.text(true);
                        ws.write(net::buffer(response));
                    }
                    else if (request["type"] == "loadProgress") {
                        GraduationProgress progress;
                        std::string moduleId;
                        auto completedCourses = progress.loadProgress(moduleId);
                        
                        json response = {
                            {"type", "savedProgress"},
                            {"moduleId", moduleId},
                            {"completedCourses", completedCourses}
                        };
                        
                        ws.text(true);
                        ws.write(net::buffer(response.dump()));
                    }
                    else if (request["type"] == "calculateCourseStatuses") {
                        CoursePlanner planner;
                        json planData = request["planData"];
                        int currentYear = request["currentYear"];
                        
                        std::string response = planner.calculateCourseStatuses(planData, currentYear);
                        ws.text(true);
                        ws.write(net::buffer(response));
                    }
                    else if (request["type"] == "loadPlanner") {
                        try {
                            CoursePlanner planner;
                            std::string response = planner.loadPlannerData();
                            std::cout << "[DEBUG] Sending planner response: " << response << std::endl;
                            ws.text(true);
                            ws.write(net::buffer(response));
                        } catch (const std::exception& e) {
                            std::cerr << "[ERROR] Error handling loadPlanner request: " << e.what() << std::endl;
                            json errorResponse = {
                                {"type", "error"},
                                {"message", std::string("Failed to load planner data: ") + e.what()}
                            };
                            ws.text(true);
                            ws.write(net::buffer(errorResponse.dump()));
                        }
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