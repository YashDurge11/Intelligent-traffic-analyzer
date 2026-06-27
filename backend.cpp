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
    int waitingTime;       // Kitni der se RED par intezar kar rahi hai
    int currentGreenTime;  // Agar GREEN hai, toh kitni der se GREEN hai
};

int main() {
    crow::SimpleApp app;

    // 4 Lanes ka initial data
    std::vector<Lane> intersection = {
        {"North Avenue", 5, "RED", 0, 0},
        {"South Street", 3, "RED", 0, 0},
        {"East Boulevard", 12, "GREEN", 0, 0},
        {"West Route", 2, "RED", 0, 0}
    };

    CROW_ROUTE(app, "/")
    ([](){
        return crow::mustache::load_text("index.html");
    });

    CROW_ROUTE(app, "/traffic-stream")
        .websocket()
        .onopen([&](crow::websocket::connection& conn) {
            std::cout << "Advanced Control Room Link Active!" << std::endl;
            
            std::thread([&conn, &intersection]() mutable {
                const int MAX_GREEN_LIMIT = 8; // Max 8 cycles tak hi koi lane GREEN reh sakti hai

                while (true) {
                    // 1. Simulation: Gadiyon ka aana-jaana
                    for (auto& lane : intersection) {
                        if (lane.lightState == "GREEN") {
                            // GREEN lane se gadiyan kam hongi
                            lane.vehicleCount = std::max(0, lane.vehicleCount - (rand() % 3));
                            lane.currentGreenTime++; // GREEN rehne ka samay badhao
                            lane.waitingTime = 0;    // Intezar khatam
                        } else {
                            // RED lane mein gadiyan badhengi aur waiting time bhi badhega
                            if (rand() % 2 == 0) lane.vehicleCount += rand() % 3;
                            lane.waitingTime += 2;    // Priority badhane ke liye waiting time jod rahe hain
                            lane.currentGreenTime = 0;
                        }
                    }

                    // Aapka pucha hua case: Naye 25 vehicles ka achanak aa jana (Random Spike)
                    if (rand() % 5 == 0) {
                        intersection[rand() % 4].vehicleCount += 25; 
                    }

                    // 2. SMART ALGORITHM: Priority Score calculate karna
                    // Formula: Score = Vehicles + (Waiting Time * 0.5)
                    int bestLaneIndex = 0;
                    double maxScore = -1.0;
                    bool forceSwitch = false;

                    for (size_t i = 0; i < intersection.size(); ++i) {
                        // Agar koi lane bohot der se GREEN hai, toh use force-stop karo (Solution 1)
                        if (intersection[i].lightState == "GREEN" && intersection[i].currentGreenTime >= MAX_GREEN_LIMIT) {
                            forceSwitch = true;
                        }

                        double score = intersection[i].vehicleCount + (intersection[i].waitingTime * 0.5);
                        if (score > maxScore) {
                            maxScore = score;
                            bestLaneIndex = i;
                        }
                    }

                    // 3. Lights Switch karna (Deadlock se bachane ke liye)
                    for (size_t i = 0; i < intersection.size(); ++i) {
                        if (forceSwitch) {
                            // Agar purani GREEN ka time khatam, toh use RED karo aur agli best lane ko GREEN karo
                            if (i == (size_t)bestLaneIndex) intersection[i].lightState = "GREEN";
                            else intersection[i].lightState = "RED";
                        } else {
                            // Normal condition mein jiska score highest hai wo GREEN
                            if (i == (size_t)bestLaneIndex && intersection[i].vehicleCount > 5) {
                                intersection[i].lightState = "GREEN";
                            } else if (i != (size_t)bestLaneIndex) {
                                intersection[i].lightState = "RED";
                            }
                        }
                    }

                    // 4. Send Data to Dashboard
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
                    
                    try { conn.send_text(jsonPayload); } catch (...) { break; }

                    std::this_thread::sleep_for(std::chrono::seconds(1)); // 1 second update rate
                }
            }).detach();
        });

    std::cout << "Traffic Analyzer running on http://localhost:18080" << std::endl;
    app.port(18080).multithreaded().run();
}
