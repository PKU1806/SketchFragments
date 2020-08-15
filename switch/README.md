/*
 * @Author: Leo Appleby 
 * @Date: 2020-08-01 16:36:28 
 * @Last Modified by: Leo Appleby
 * @Last Modified time: 2020-08-08 20:57:04
 */

# 文件/文件夹介绍
folder：
1. include：部份p4的源代码，包括header和parser
   1. header.p4
   2. parser.p4
2. p4src_flowsize:用于flowsize实验的代码
   1. ecmp.p4:p4源代码
   2. p4app.json:p4配置文件
   3. log文件夹:mininet自动生成
   4. pcap文件夹:mininet自动生成
   5. ecmp.json:mininet自动生成
   6. ecmp.p4i:mininet自动生成
   7. topology.db:mininet自动生成
3. p4src_interval:用于包间隔实验的代码(介绍同上)
4. scripts：各类python脚本
   1. error.py：生成loop，blackhole或者重置路由表
   2. log_generator.py：生成switch上的log
   3. monitor.py：用于进行sketch的切换
   4. routing-controller.py：基本的路由表配置，寄存器配置，hash函数配置等
   5. simple_receiver.py：简单的收包脚本，可简单解析包
   6. simple_sender.py：简单的发包脚本，支持三种协议
   7. visor.py：打印所有的sketch内部的信息
   
5. switch_log:运行时使用脚本会生成，记录了switch上的信息



# 使用方法
## 基本步骤 
1. get into the `../switch/p4src_……` folder
2. activate tmux and create duplicate windows
3. window A as the mininet console ,type ` sudo p4run ` to evoke the mininet
4. when the mininet_env is established, type `python routing-controller.py`  to set the tables in window B

## send packet & receive packet & listen to switch
5. in window C,type `mx h1` and type `python simple_sender.py -h` to send packet from h1 and get help
6. in window D,type `mx h4` and type `python simple_receiver.py` to listen from a eth port
7. in window B (after mini-step4),type `mx s7` ,and `python test.py -h` to minitor control plane of s7 


## question & problems 
1. why icmp will fail when s1 shutdown while udp will be ok: 1. ecmp 2. parser 3.the send back?
   SOLVED:by add a item in the hash of ecmp;
   BUT mininet `pingall` instruction may show that some hosts are still not connected yet, but using our sender/receiver,we can prove they are actually connected via tcp icmp and udp 
2. the packet's compatibility：solved


