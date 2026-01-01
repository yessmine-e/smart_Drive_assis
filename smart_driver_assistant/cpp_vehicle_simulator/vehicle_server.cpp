#include <iostream>
#include <thread>
#include <chrono>
#include <mutex>
#include "libs/httplib.h"

// Simple clamp helper
double clamp(double value, double min_val, double max_val) {
    if (value < min_val) return min_val;
    if (value > max_val) return max_val;
    return value;
}

// Shared state
struct VehicleState {
    double speed_kmh;
    double outside_temp_c;
    double cabin_temp_c;
    double battery_level_percent;
};

VehicleState g_state;
std::mutex g_mutex;

// Background thread: update signals
void simulation_loop() {
    double speed_kmh = 0.0;
    double outside_temp_c = 30.0;
    double cabin_temp_c = 27.0;
    double battery_level_percent = 100.0;
    bool accelerating = true;

    while (true) {
        if (accelerating) {
            speed_kmh += 5.0;
            if (speed_kmh >= 120.0) accelerating = false;
        } else {
            speed_kmh -= 5.0;
            if (speed_kmh <= 20.0) accelerating = true;
        }
        speed_kmh = clamp(speed_kmh, 0.0, 140.0);

        outside_temp_c += 0.1;
        if (outside_temp_c > 35.0) outside_temp_c = 25.0;

        if (cabin_temp_c < outside_temp_c)
            cabin_temp_c += 0.2;
        else
            cabin_temp_c -= 0.1;
        cabin_temp_c = clamp(cabin_temp_c, 18.0, 40.0);

        battery_level_percent -= (speed_kmh > 0 ? 0.05 : 0.01);
        if (battery_level_percent < 5.0) battery_level_percent = 100.0;

        {
            std::lock_guard<std::mutex> lock(g_mutex);
            g_state.speed_kmh = speed_kmh;
            g_state.outside_temp_c = outside_temp_c;
            g_state.cabin_temp_c = cabin_temp_c;
            g_state.battery_level_percent = battery_level_percent;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
}

int main() {
    // Start simulation thread
    std::thread sim_thread(simulation_loop);

    // HTTP server on localhost:8080
    httplib::Server svr;

    svr.Get("/signals", [](const httplib::Request&, httplib::Response& res) {
        VehicleState state_copy;
        {
            std::lock_guard<std::mutex> lock(g_mutex);
            state_copy = g_state;
        }

        std::string json = "{"
            "\"speed_kmh\":" + std::to_string(state_copy.speed_kmh) + ","
            "\"outside_temp_c\":" + std::to_string(state_copy.outside_temp_c) + ","
            "\"cabin_temp_c\":" + std::to_string(state_copy.cabin_temp_c) + ","
            "\"battery_level_percent\":" + std::to_string(state_copy.battery_level_percent) +
            "}";

        res.set_content(json, "application/json");
    });

    std::cout << "Server running on http://localhost:8080/signals\n";
    svr.listen("0.0.0.0", 8080);

    sim_thread.join();
    return 0;
}
