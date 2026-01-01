from flask import Flask, jsonify
import json

app = Flask(__name__)

SIGNALS_PATH = r"E:\smart_driver_assistant\cpp_vehicle_simulator\signals.json"

@app.route("/signals", methods=["GET"])
def get_signals():
    with open(SIGNALS_PATH, "r") as f:
        data = json.load(f)
    return jsonify(data)  # JSON with speed_kmh, temps, battery

if __name__ == "__main__":
    app.run(host="0.0.0.0", port=5000)
