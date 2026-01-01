import pandas as pd
import numpy as np
import tensorflow as tf
from tensorflow import keras
from tensorflow.keras import layers
from sklearn.model_selection import train_test_split

# 1. Load dataset
df = pd.read_csv("dataset.csv")

# Optional: drop timestamp
df = df.drop(columns=["timestamp"])

# 2. Split into features (X) and labels (y)
feature_cols = ["speed_kmh", "outside_temp_c", "cabin_temp_c", "battery_level_percent"]
X = df[feature_cols].values.astype("float32")
y = df["label"].values.astype("int32")

# 3. Train/validation split
X_train, X_val, y_train, y_val = train_test_split(
    X, y, test_size=0.2, random_state=42, stratify=y
)

# 4. Build small dense model (4 inputs -> 4 classes)
num_features = X_train.shape[1]
num_classes = len(np.unique(y))

model = keras.Sequential(
    [
        layers.Input(shape=(num_features,)),
        layers.Dense(16, activation="relu"),
        layers.Dense(16, activation="relu"),
        layers.Dense(num_classes, activation="softmax"),
    ]
)

model.compile(
    optimizer="adam",
    loss="sparse_categorical_crossentropy",  # labels are 0,1,2,3
    metrics=["accuracy"],
)

model.summary()

# 5. Train
history = model.fit(
    X_train,
    y_train,
    validation_data=(X_val, y_val),
    epochs=30,
    batch_size=32,
    verbose=1,
)

# 6. Evaluate
val_loss, val_acc = model.evaluate(X_val, y_val, verbose=0)
print(f"Validation accuracy: {val_acc:.3f}")

# 7. Save Keras model
model.save("ev_assistant_model.h5")

# 8. Convert to TFLite
converter = tf.lite.TFLiteConverter.from_keras_model(model)
tflite_model = converter.convert()

with open("ev_assistant_model.tflite", "wb") as f:
    f.write(tflite_model)

print("Saved ev_assistant_model.tflite")
