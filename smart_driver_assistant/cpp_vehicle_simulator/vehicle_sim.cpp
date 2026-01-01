#include <iostream>
#include <thread>
#include <chrono>
#include <fstream>
#include <string>

// Simple clamp helper
double clamp(double value, double min_val, double max_val) {
    if (value < min_val) return min_val;
    if (value > max_val) return max_val;
    return value;
}

// Helper to smoothly move a value toward a target
double move_towards(double value, double target, double step) {
    if (value < target) {
        value += step;
        if (value > target) value = target;
    } else if (value > target) {
        value -= step;
        if (value < target) value = target;
    }
    return value;
}

int main() {
    // State variables
    double speed_kmh = 0.0;
    double outside_temp_c = 25.0;
    double cabin_temp_c = 24.0;
    double battery_level_percent = 100.0;

    // Mode: 0=normal, 1=comfort (hot cabin), 2=safety (high speed), 3=energy (low battery)
    int mode = 0;
    int steps_in_mode = 0;
    const int MODE_DURATION_STEPS = 200; // ~100 seconds if sleep 500 ms

    while (true) {
        // --- Switch mode periodically ---
        if (steps_in_mode >= MODE_DURATION_STEPS) {
            mode = (mode + 1) % 4; // cycle 0 -> 1 -> 2 -> 3 -> 0 ...
            steps_in_mode = 0;
            std::cout << "Switching to mode " << mode << std::endl;
        }
        steps_in_mode++;

        // --- Set target ranges per mode ---
        double target_speed = 0.0;
        double target_outside = outside_temp_c;
        double target_cabin = cabin_temp_c;
        double target_battery = battery_level_percent;

        switch (mode) {
            case 0: // normal: moderate speed, comfortable cabin, high battery
                target_speed = 60.0;                 // 40–90
                target_outside = 25.0;              // mild weather
                target_cabin = 24.0;                // comfortable
                target_battery = 90.0;              // high SOC
                break;

            case 1: // comfort_tip: hot cabin
                target_speed = 70.0;                // normal speed
                target_outside = 33.0;              // hot outside
                target_cabin = 34.0;                // > 30 °C
                target_battery = 80.0;              // still high
                break;

            case 2: // safety_tip: high speed
                target_speed = 140.0;               // > 120 km/h
                target_outside = 26.0;
                target_cabin = 24.0;
                target_battery = 70.0;
                break;

            case 3: // energy_tip: low battery
                target_speed = 50.0;                // moderate speed
                target_outside = 25.0;
                target_cabin = 24.0;
                target_battery = 10.0;              // < 20 %
                break;
        }

        // --- Smoothly move current state toward target ---
        speed_kmh = move_towards(speed_kmh, target_speed, 5.0);
        speed_kmh = clamp(speed_kmh, 0.0, 160.0);

        outside_temp_c = move_towards(outside_temp_c, target_outside, 0.2);
        outside_temp_c = clamp(outside_temp_c, -10.0, 45.0);

        cabin_temp_c = move_towards(cabin_temp_c, target_cabin, 0.3);
        cabin_temp_c = clamp(cabin_temp_c, 15.0, 45.0);

        // For battery: move toward target but also drain slightly with speed
        double drain = (speed_kmh > 0 ? 0.02 : 0.005);
        battery_level_percent -= drain;
        battery_level_percent = move_towards(battery_level_percent, target_battery, 0.5);
        if (battery_level_percent < 3.0) battery_level_percent = 100.0; // reset if too low
        battery_level_percent = clamp(battery_level_percent, 0.0, 100.0);

        // --- Build JSON string ---
        std::string json =
            "{"
            "\"speed_kmh\":" + std::to_string(speed_kmh) + ","
            "\"outside_temp_c\":" + std::to_string(outside_temp_c) + ","
            "\"cabin_temp_c\":" + std::to_string(cabin_temp_c) + ","
            "\"battery_level_percent\":" + std::to_string(battery_level_percent) +
            "}";

        // Print to console
        std::cout << json << std::endl;

        // Write latest snapshot to file (adjust path if needed)
        std::ofstream ofs("E:\\smart_driver_assistant\\cpp_vehicle_simulator\\signals.json");
        if (!ofs) {
            std::cerr << "ERROR: cannot open signals.json for writing" << std::endl;
        } else {
            ofs << json;
        }
        ofs.close();

        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }

    return 0;
}
