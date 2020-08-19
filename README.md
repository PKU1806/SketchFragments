# 基本用法
1. 进入SketchFragment文件夹
2. 使用tmux开至少三个窗口
3. 窗口A进入switch文件夹中对应文件夹：p4src_flowsize/p4src_interval
4. 窗口A键入`sudo p4run`
5. 窗口B进入switch文件夹中script文件夹
6. 窗口B等待窗口Amininet启动后，键入`python routing_controller.py f`启动flowsize的配置脚本，或者键入`python routing_controller.py i`启动interval的配置脚本
7. 至此，mininet基本环境已配置完成
8. 窗口C进入switch文件夹中script文件夹，键入`python log_generator.py {f/i}`启动switch端的log 生成脚本


# p4app.json配置注意项：
1. 更改program项，使得其对应文件正确
2. 于switches项中每一个switch后添加`"cpu_port" : true`,如：`"s1": {"cpu_port" : true}`

# sketch 切换

# error生成
