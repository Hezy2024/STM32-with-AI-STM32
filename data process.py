import pandas as pd

# 读取 CSV 文件，假设数据用空格分隔
df = pd.read_csv('左右左右.csv', header=None, delim_whitespace=True)

# 将第一列的数据按空格分开
expanded_data = df[0].astype(str).str.split(expand=True)

# 将分开的数据合并回原数据框
for i in range(expanded_data.shape[1]):
    df[i + 1] = expanded_data[i].astype(float)  # 确保转换为浮点数

# 保存结果到新的 CSV 文件
df.to_csv('modified_data_left_right.csv', index=False, header=False)
