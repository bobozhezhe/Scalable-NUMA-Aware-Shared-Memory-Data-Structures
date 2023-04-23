import pandas as pd
import matplotlib.pyplot as plt

# 读取 CSV 文件
data = pd.read_csv('local_results.csv')

# 循环遍历每个 container，对于每个 container 都生成一个子图
for container in data['container'].unique():
    fig, ax = plt.subplots()
    ax.set_title(container)

    # 选择该 container 的数据
    container_data = data[data['container'] == container]

    # 循环遍历每个 datatype，对于每个 datatype 都生成一个 bar group
    position = 0
    for datatype in container_data['datatype'].unique():
        datatype_data = container_data[container_data['datatype'] == datatype]

        # 循环遍历每个 operation，对于每个 operation 都生成一个 bar
        for operation in datatype_data['operation'].unique():
            operation_data = datatype_data[datatype_data['operation'] == operation]

            if operation == 'push_back':
                color = 'r'
            elif operation == 'read':
                color = 'g'
            elif operation == 'erase':
                color = 'b'

            ax.bar(position, operation_data['time(us)'], color=color, label=operation)
            position += 1

        position += 2

    ax.set_xlabel('datatype')
    ax.set_xticks(range(position))
    ax.set_xticklabels(container_data['datatype'].unique())
    ax.set_ylabel('time(us)')
    ax.legend()

plt.show()
