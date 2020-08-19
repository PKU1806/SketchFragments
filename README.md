# 基本用法
1. 进入SketchFragment文件夹
2. 使用tmux开至少三个窗口
3. 窗口A进入switch文件夹中对应文件夹：p4src_flowsize/p4src_interval
4. 窗口A键入`sudo p4run`
5. 窗口B进入switch文件夹中script文件夹
6. 窗口B等待窗口Amininet启动后，键入`python routing_controller.py f`启动flowsize的配置脚本，或者键入`python routing_controller.py i`启动interval的配置脚本
7. 至此，mininet基本环境已配置完成
8. 窗口C进入switch文件夹中script文件夹，先键入`mx sw-cpu`,再键入`python log_generator.py {f/i}`启动switch端的log 生成脚本，log位于switch/switch_log
9. 先在client_flowsize/client_interval文件夹下新建一个名为download_sketch的文件夹，用于存储搜集到的sketch碎片
10. 窗口D进入client文件夹，进入对应的client_flowsize/client_interval文件夹，键入`make`，键入`sudo ./traffic conn_num`,其中conn_num用于指定生成流的数量,至此收发包程序启动完成
11. 在client_flowsize/client_interval文件夹下会生成一个名为flow.txt的文件，记录traffic生成的流的信息
12. 在client_flowsize/download_sketch文件夹中，会生成记录着flowsize碎片信息的txt文件，例如switch1_array1.txt，表示来自switch1的sketch的array1，其中每行表示一个bucket；在client_interval/download_sketch文件夹中，会生成记录着interval碎片信息的txt文件，例如switch1.txt，表示来自switch1的sketch，其中每行表示一个array

# p4app.json配置注意项：
1. 更改program项，使得其对应文件正确
2. 于switches项中每一个switch后添加`"cpu_port" : true`,如：`"s1": {"cpu_port" : true}`
3. 不同的topology下需要根据switch数目于switch/script/log_generator.py的main函数中增加switch监听线程

# sketch 切换
启动switch/script/monitor.py
# error生成
启动switch/script/error.py
# 简易scapy收发包
配合mx命令，启动switch/script/simple_sender.py  switch/script/simple_receiver.py 
# hint 
switch/script 下的脚本大多具有帮助信息
