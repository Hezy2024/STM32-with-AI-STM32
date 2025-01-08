#模型训练文件mini_demo.py 训练 epochs 1000次

'''
开发板（正面朝上）姿态检测
静止不动、左右移动、上下移动
 
输入层 -> 隐藏层 -> 输出层
'''
 
import pandas as pd
import numpy as np
import torch
import torch.nn as nn
import torch.optim as optim
from torch.utils.data import DataLoader, TensorDataset, random_split
from sklearn.preprocessing import MinMaxScaler

# %% 读取数据
data = pd.read_csv('modified_data_xyz.csv', sep=',', header=None)
data_x = data.iloc[:, 0:9].values.astype(float)  # 取前9列
data_y = data.iloc[:, 9:12].values.astype(int)  # 取第10到第12列作为目标值（三位0/1编码）

# %% 数据归一化
scaler = MinMaxScaler()
data_x_normalized = scaler.fit_transform(data_x)  # 归一化处理

# 将数据转换为 PyTorch 张量
X_tensor = torch.tensor(data_x_normalized, dtype=torch.float32)  # 特征
y_tensor = torch.tensor(data_y, dtype=torch.float32)  # 目标为浮点型

# 创建数据集
dataset = TensorDataset(X_tensor, y_tensor)

# %% 数据集划分
train_size = int(0.7 * len(dataset))  # 70% 训练集
val_size = len(dataset) - train_size  # 30% 验证集
train_dataset, val_dataset = random_split(dataset, [train_size, val_size])

# 创建数据加载器
train_loader = DataLoader(train_dataset, batch_size=72, shuffle=True)
val_loader = DataLoader(val_dataset, batch_size=72, shuffle=False)

# %% 定义前馈神经网络模型
class FeedForwardNN(nn.Module):
    def __init__(self):
        super(FeedForwardNN, self).__init__()
        self.fc1 = nn.Linear(9, 128)  # 输入层到隐藏层
        self.fc2 = nn.Linear(128, 64)  # 隐藏层到隐藏层
        self.fc3 = nn.Linear(64, 3)    # 隐藏层到输出层（3 类）

    def forward(self, x):
        x = torch.relu(self.fc1(x))  # 第一层
        x = torch.relu(self.fc2(x))  # 第二层
        x = self.fc3(x)               # 输出层
        return x

# 实例化模型
model = FeedForwardNN()

# 定义损失函数和优化器
criterion = nn.BCEWithLogitsLoss()  # 使用二元交叉熵损失
optimizer = optim.SGD(model.parameters(), lr=0.01, momentum=0.9)  # 设置学习率

# %% 训练模型
epochs = 1000
for epoch in range(epochs):
    total_loss = 0
    total_correct = 0
    total_samples = 0
    
    model.train()  # 切换到训练模式
    for inputs, labels in train_loader:
        optimizer.zero_grad()  # 清零梯度
        outputs = model(inputs)  # 前向传播
        loss = criterion(outputs, labels)  # 计算损失
        loss.backward()  # 反向传播
        optimizer.step()  # 更新参数
        
        # 计算准确率（通过阈值将输出转为分类）
        predicted = (torch.sigmoid(outputs) > 0.5).float()
        total_correct += (predicted == labels).sum().item()
        total_samples += labels.size(0) * labels.size(1)  # 计算总样本数
        total_loss += loss.item()
    
    accuracy = total_correct / total_samples
    print(f'Epoch [{epoch + 1}/{epochs}], Loss: {total_loss:.4f}, Accuracy: {accuracy * 100:.2f}%')

# %% 模型评估
model.eval()  # 切换到评估模式
total_val_correct = 0
total_val_samples = 0

with torch.no_grad():
    for inputs, labels in val_loader:
        outputs = model(inputs)  # 前向传播
        predicted = (torch.sigmoid(outputs) > 0.5).float()  # 获取预测结果
        total_val_correct += (predicted == labels).sum().item()
        total_val_samples += labels.size(0) * labels.size(1)  # 计算总样本数

val_accuracy = total_val_correct / total_val_samples
print(f'Validation Accuracy: {val_accuracy * 100:.2f}%')

# 保存模型参数
torch.save(model.state_dict(), 'feedforward_model.pth')
print("Model parameters saved.")