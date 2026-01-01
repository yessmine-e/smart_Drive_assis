package com.example.smartdriverassistant.network

data class VehicleState(
    val speed_kmh: Float,
    val outside_temp_c: Float,
    val cabin_temp_c: Float,
    val battery_level_percent: Float
)
