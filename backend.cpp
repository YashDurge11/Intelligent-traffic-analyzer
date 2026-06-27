#include "crow.h"
#include <cstdlib>
#include <chrono>
#include <string>
#include <thread>
#include <algorithm>
#include <vector>

struct Lane {
    std::string name;
    int vehicleCount;
    std::string lightState;
};

int main() {
    crow::SimpleApp app;

    std::vector<Lane> intersection = {
        {"North Avenue", 5, "RED"},
        {"South Street", 3, "RED"},
        {"East Boulevard", 12, "GREEN"},
        {"West Route", 2, "RED"}
    };

    CROW_ROUTE(app, "/")
    ([](){
        return crow::mustache::load_text("index.html");
    });

    CROW_ROUTE(app, "/traffic-stream")
        .websocket()
        .onopen([&](crow::websocket::connection& conn) {
            std::cout << "Control Room Link Established!" << std::endl;
            
            std::thread([&conn, &intersection]() mutable {
                while (true) {
                    for (auto& lane : intersection) {
                        int delta = (rand() % 3) - 1; 
                        lane.vehicleCount = std::max(0, lane.vehicleCount + delta);
                        if (rand() % 10 == 0) lane.vehicleCount += rand() % 5;
                    }

                    auto maxLaneIt = std::max_element(intersection.begin(), intersection.end(), 
                        [](const Lane& a, const Lane& b) { return a.vehicleCount < b.vehicleCount; });

                    for (auto& lane : intersection) {
                        if (lane.name == maxLaneIt->name && lane.vehicleCount > 7) {
                            lane.lightState = "GREEN";
                        } else {
                            lane.lightState = "RED";
                        }
                    }

                    std::string jsonPayload = "[";
                    for (size_t i = 0; i < intersection.size(); ++i) {
                        jsonPayload += "{"
                            "\"lane\":\"" + intersection[i].name + "\","
                            "\"vehicles\":" + std::to_string(intersection[i].vehicleCount) + ","
                            "\"light\":\"" + intersection[i].lightState + "\""
                            "}";
                        if (i < intersection.size() - 1) jsonPayload += ",";
                    }
                    jsonPayload += "]";
                    
                    try {
                        conn.send_text(jsonPayload);
                    } catch (...) {
                        break; 
                    }

                    std::this_thread::sleep_for(std::chrono::milliseconds(900));
                }
            }).detach();
        });

    std::cout << "Traffic Analyzer active on http://localhost:18080" << std::endl;
    app.port(18080).multithreaded().run();
}
