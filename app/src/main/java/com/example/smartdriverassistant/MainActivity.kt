package com.example.smartdriverassistant

import android.os.Bundle
import androidx.activity.ComponentActivity
import androidx.activity.compose.setContent
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.padding
import androidx.compose.material3.Button
import androidx.compose.material3.ButtonDefaults
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.Surface
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.runtime.LaunchedEffect
import androidx.compose.runtime.getValue
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.remember
import androidx.compose.runtime.setValue
import androidx.compose.ui.Modifier
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.unit.dp
import com.example.smartdriverassistant.network.RetrofitClient
import com.example.smartdriverassistant.network.VehicleState
import kotlinx.coroutines.delay

class MainActivity : ComponentActivity() {
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)

        val interpreter = EvAssistantInterpreter(this)

        setContent {
            SmartDriverAssistantScreen(interpreter)
        }
    }
}

@Composable
fun SmartDriverAssistantScreen(interpreter: EvAssistantInterpreter) {

    var vehicleState by remember {
        mutableStateOf(
            VehicleState(
                speed_kmh = 0f,
                outside_temp_c = 25f,
                cabin_temp_c = 24f,
                battery_level_percent = 100f
            )
        )
    }
    var adviceText by remember { mutableStateOf("Waiting for data...") }
    var currentMode by remember { mutableStateOf("Normal") }
    var isRunning by remember { mutableStateOf(true) }
    var errorText by remember { mutableStateOf<String?>(null) }

    LaunchedEffect(isRunning) {
        while (isRunning) {
            try {
                val data = RetrofitClient.api.getSignals()
                vehicleState = data
                errorText = null

                val finalLabel = when {
                    data.battery_level_percent < 5f -> 3
                    data.speed_kmh > 130f -> 2
                    else -> interpreter.predictLabel(
                        data.speed_kmh,
                        data.outside_temp_c,
                        data.cabin_temp_c,
                        data.battery_level_percent
                    )
                }

                when (finalLabel) {
                    1 -> {
                        currentMode = "Comfort"
                        adviceText = "Comfort tip: Cabin temperature not optimal."
                    }
                    2 -> {
                        currentMode = "Safety"
                        adviceText = "Safety tip: Speed is high, consider slowing down."
                    }
                    3 -> {
                        currentMode = "Energy"
                        adviceText = "Energy tip: Battery is low, drive in eco style."
                    }
                    else -> {
                        currentMode = "Normal"
                        adviceText = "Conditions are normal."
                    }
                }
            } catch (e: Exception) {
                errorText = "Simulateur non connecté"
            }

            delay(1000)
        }
    }

    val modeColor = when (currentMode) {
        "Comfort" -> Color(0xFFFFA726) // orange
        "Safety"  -> Color(0xFFE53935) // rouge
        "Energy"  -> Color(0xFF1E88E5) // bleu
        else      -> Color(0xFF43A047) // vert
    }

    Surface(
        modifier = Modifier.fillMaxSize(),
        color = MaterialTheme.colorScheme.background
    ) {
        Column(modifier = Modifier.padding(16.dp)) {

            Text(text = "Mode: $currentMode", color = modeColor)
            Text(text = "Speed: ${vehicleState.speed_kmh} km/h")
            Text(text = "Outside temp: ${vehicleState.outside_temp_c} °C")
            Text(text = "Cabin temp: ${vehicleState.cabin_temp_c} °C")
            Text(text = "Battery: ${vehicleState.battery_level_percent} %")

            Text(
                text = "Advice:\n$adviceText",
                modifier = Modifier.padding(top = 16.dp),
                color = modeColor
            )

            if (errorText != null) {
                Text(
                    text = errorText!!,
                    color = Color.Red,
                    modifier = Modifier.padding(top = 8.dp)
                )
            }

            Button(
                onClick = { isRunning = !isRunning },
                modifier = Modifier.padding(top = 24.dp),
                colors = ButtonDefaults.buttonColors(
                    containerColor = if (isRunning) Color.Red else Color.Green
                )
            ) {
                Text(if (isRunning) "Stop simulation" else "Start simulation")
            }
        }
    }
}
