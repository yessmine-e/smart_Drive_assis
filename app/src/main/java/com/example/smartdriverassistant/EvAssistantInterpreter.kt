package com.example.smartdriverassistant

import android.content.Context
import org.tensorflow.lite.Interpreter
import java.io.FileInputStream
import java.nio.MappedByteBuffer
import java.nio.channels.FileChannel

class EvAssistantInterpreter(context: Context) {

    companion object {
        private const val MODEL_FILE = "ev_assistant_model.tflite"
    }

    private val interpreter: Interpreter

    init {
        val modelBuffer = loadModelFile(context)
        interpreter = Interpreter(modelBuffer)
    }

    private fun loadModelFile(context: Context): MappedByteBuffer {
        // Charge le modèle depuis app/src/main/ml/
        val fileDescriptor = context.assets.openFd(MODEL_FILE)
        val inputStream = FileInputStream(fileDescriptor.fileDescriptor)
        val fileChannel = inputStream.channel
        val startOffset = fileDescriptor.startOffset
        val declaredLength = fileDescriptor.declaredLength
        return fileChannel.map(FileChannel.MapMode.READ_ONLY, startOffset, declaredLength)
    }

    fun predictLabel(
        speedKmh: Float,
        outsideTempC: Float,
        cabinTempC: Float,
        batteryPercent: Float
    ): Int {
        val input = arrayOf(
            floatArrayOf(speedKmh, outsideTempC, cabinTempC, batteryPercent)
        )
        val output = Array(1) { FloatArray(4) }

        interpreter.run(input, output)

        val probs = output[0]
        var maxIdx = 0
        var maxVal = probs[0]
        for (i in 1 until probs.size) {
            if (probs[i] > maxVal) {
                maxVal = probs[i]
                maxIdx = i
            }
        }
        return maxIdx
    }

    fun close() {
        interpreter.close()
    }
}
