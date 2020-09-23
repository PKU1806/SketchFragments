# Experimental Implementation, Simulation and Analysis

## Usage

- 使用mininet跑出switch log，流程：启动mininet、配置路由表、sw-cpu开启log、微调整拓扑以满足任务要求、开启traffic生成流

- 使用analysis，判断实验路由log收集情况、丢包情况，并生成sketch可读入的trace

- 使用sketch，进行benchmark，与其它算法做比较

## Folders

### topology

生成对应参数的fattree拓扑，格式为p4输入的json

### plt

在Jupiter Notebook中利用log中的数据画图

### sketch

包含半CU等sketch的模拟，可以直接读取analysis中输出的trace作为数据集(见`main.cpp`)

### analysis

包含对于switch log的分析，以及基于log的数据集生成。需要注意client端与switch端对于一个switch的名字表述不同，需要`sw_map`文件做映射。

### sim

直接生成模拟的trace（analysis需要先从mininet获得log才能转成trace），也支持生成analysis可读取的log