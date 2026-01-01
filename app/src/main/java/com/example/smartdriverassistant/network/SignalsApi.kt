package com.example.smartdriverassistant.network

import retrofit2.http.GET

interface SignalsApi {
    @GET("signals")
    suspend fun getSignals(): VehicleState
}
