import pandas as pd
import numpy as np
import tensorflow as tf
from sklearn.preprocessing import MinMaxScaler
from sklearn.model_selection import train_test_split

# %% 读取数据
data = pd.read_csv('modified_data_xyz.csv', sep=',', header=None)
data_x = data.iloc[:, 0:9].values.astype(float)  # 取前9列
data_y = data.iloc[:, 9:12].values.astype(int)  # 取第10到第12列作为目标值（三位0/1编码）

# %% 数据归一化
scaler = MinMaxScaler()
data_x_normalized = scaler.fit_transform(data_x)  # 归一化处理

# %% 划分训练集和验证集
X_train, X_val, y_train, y_val = train_test_split(data_x_normalized, data_y, test_size=0.3, random_state=42)

# %% 定义 Keras Sequential 模型
model = tf.keras.Sequential([
    tf.keras.layers.Dense(128, activation='relu', input_shape=(9,)),
    tf.keras.layers.Dense(64, activation='relu'),
    tf.keras.layers.Dense(3, activation='sigmoid')
])

# %% 编译模型
model.compile(optimizer='sgd', loss='binary_crossentropy', metrics=['accuracy'])

# %% 训练模型
epochs = 1500
history = model.fit(X_train, y_train, epochs=epochs, batch_size=72, validation_data=(X_val, y_val))

# %% 模型评估
val_loss, val_accuracy = model.evaluate(X_val, y_val)
print(f'Validation Loss: {val_loss:.4f}, Validation Accuracy: {val_accuracy * 100:.2f}%')

# %% 保存模型为 .h5 格式
model.save('feedforward_model.h5')
print("Model saved as feedforward_model.h5.")

# %% 加载模型
loaded_model = tf.keras.models.load_model('feedforward_model.h5')
print("Model loaded successfully.")