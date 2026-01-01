Project overview
The system simulates EV driving conditions on a PC, streams the signals to an Android app, and uses a TensorFlow Lite model plus rule‑based logic to provide comfort, safety, and energy‑saving advice to the driver in real time.
​

Main components
C++ vehicle simulator (PC)

Generates realistic vehicle signals: speed, outside temperature, cabin temperature, battery state of charge.

Runs continuously and writes the latest snapshot to a signals.json file.

Python/Flask bridge (PC)

Lightweight REST API that exposes /signals.

Reads signals.json and returns the current vehicle state as JSON over HTTP for the Android device.
​

Android application (phone)

Implemented in Kotlin with Jetpack Compose for the UI.

Uses Retrofit + coroutines to poll the Flask API periodically and display live vehicle data.
​

Embeds a TensorFlow Lite model that classifies the situation into Normal / Comfort / Safety / Energy modes and shows color‑coded driver advice.

Adds rule‑based overrides (e.g., very low battery, very high speed) to guarantee safe recommendations.

Technologies and tools
Languages: C++, Kotlin, Python

AI: TensorFlow Lite for on‑device inference

Android: Jetpack Compose UI, Retrofit, Kotlin coroutines

Backend/Bridge: Flask (Python) REST API

Simulation: Custom C++ signal generator (conceptually mirroring CAN/vehicle signals)

Environment: Android Studio, Visual Studio (for C++), VS Code or terminal for Flask

This setup demonstrates an end‑to‑end pipeline from vehicle signal simulation to real‑time AI‑driven driver assistance on an Android device, using technologies that are close to those employed in Android Automotive systems while remaining feasible for a student project.
