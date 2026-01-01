import json
import time
import csv
import os
from datetime import datetime

SIGNALS_PATH = r"E:\smart_driver_assistant\cpp_vehicle_simulator\signals.json"
ADVICE_PATH = "advice.json"
CSV_PATH = "dataset.csv"


def init_csv():
    """Create CSV file with header if it does not exist."""
    file_exists = os.path.isfile(CSV_PATH)
    if not file_exists:
        with open(CSV_PATH, mode="w", newline="") as f:
            writer = csv.DictWriter(
                f,
                fieldnames=[
                    "timestamp",
                    "speed_kmh",
                    "outside_temp_c",
                    "cabin_temp_c",
                    "battery_level_percent",
                    "label",
                ],
            )
            writer.writeheader()  # one-time header row [web:193][web:196]


def compute_label(speed_kmh, outside_temp_c, cabin_temp_c, battery_level_percent):
    """
    Map current state to a class label:
    0 = normal
    1 = comfort_tip (cabin too hot/cold)
    2 = safety_tip (speed too high)
    3 = energy_tip (battery low)
    Priority: safety > energy > comfort > normal.
    """
    if speed_kmh > 120.0:
        return 2  # safety_tip
    if battery_level_percent < 20.0:
        return 3  # energy_tip
    if cabin_temp_c > 30.0 or cabin_temp_c < 18.0:
        return 1  # comfort_tip
    return 0  # normal


def log_sample(speed_kmh, outside_temp_c, cabin_temp_c, battery_level_percent, label):
    """Append one labeled sample to dataset.csv."""
    with open(CSV_PATH, mode="a", newline="") as f:
        writer = csv.DictWriter(
            f,
            fieldnames=[
                "timestamp",
                "speed_kmh",
                "outside_temp_c",
                "cabin_temp_c",
                "battery_level_percent",
                "label",
            ],
        )
        writer.writerow(
            {
                "timestamp": datetime.now().isoformat(),
                "speed_kmh": speed_kmh,
                "outside_temp_c": outside_temp_c,
                "cabin_temp_c": cabin_temp_c,
                "battery_level_percent": battery_level_percent,
                "label": label,
            }
        )  # DictWriter is standard for structured CSV logging. [web:187][web:190]


def read_signals():
    """Read current signals from signals.json."""
    with open(SIGNALS_PATH, "r") as f:
        data = json.load(f)  # loads JSON into Python dict. [web:199][web:204]
    return data


def write_advice(advice_payload):
    """Write advice to advice.json (for future use / other components)."""
    with open(ADVICE_PATH, "w") as f:
        json.dump(advice_payload, f, indent=2)


def main():
    print("Assistant running with CSV logging. Press Ctrl+C to stop.")
    init_csv()

    try:
        while True:
            # 1. Read signals
            signals = read_signals()

            speed_kmh = signals.get("speed_kmh", 0.0)
            outside_temp_c = signals.get("outside_temp_c", 0.0)
            cabin_temp_c = signals.get("cabin_temp_c", 0.0)
            battery_level_percent = signals.get("battery_level_percent", 0.0)

            # 2. Apply rule-based logic for advice
            advices = []

            if cabin_temp_c > 30.0:
                advices.append("Cabin is hot. Suggest lowering AC temperature.")
            elif cabin_temp_c < 18.0:
                advices.append("Cabin is cold. Suggest increasing AC temperature.")

            if speed_kmh > 120.0:
                advices.append("Speed is high. Consider slowing down for safety.")

            if battery_level_percent < 20.0:
                advices.append(
                    "Battery is low. Suggest enabling eco mode and reducing acceleration."
                )

            if not advices:
                advices.append("Conditions are normal. No specific advice.")

            # 3. Compute label for ML dataset
            label = compute_label(
                speed_kmh, outside_temp_c, cabin_temp_c, battery_level_percent
            )

            # 4. Log to CSV
            log_sample(
                speed_kmh, outside_temp_c, cabin_temp_c, battery_level_percent, label
            )

            # 5. Optionally write advice.json for other components
            advice_payload = {
                "speed_kmh": speed_kmh,
                "outside_temp_c": outside_temp_c,
                "cabin_temp_c": cabin_temp_c,
                "battery_level_percent": battery_level_percent,
                "advices": advices,
                "label": label,
            }
            write_advice(advice_payload)

            # 6. Print to console for debugging
            print(advice_payload)

            # 7. Wait a bit before next iteration
            time.sleep(1)  # 1 second; adjust to match simulator rate. [web:206][web:212]

    except KeyboardInterrupt:
        print("Stopped by user.")


if __name__ == "__main__":
    main()
