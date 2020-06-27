import socket
import sys
import numpy as np

sys.path.append('../')

from utils.utils import String2Tuple
from client.config import bind_addr, buck_num, default_value

sketch = np.zeros(buck_num)
sketch -= 1.0
def main():
    # 创建udp套接字,
    # AF_INET表示ip地址的类型是ipv4，
    # SOCK_DGRAM表示传输的协议类型是udp
    udp_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

    # 绑定本地信息，若不绑定，系统会自动分配

    udp_socket.bind(bind_addr) # ip和port，ip一般不用写，表示本机的任何一个ip
 
    while True:
        # 等待接收数据
        revc_data  = udp_socket.recvfrom(1024)  # 1024表示本次接收的最大字节数
 
        # 打印接收到的数据
        print ("recv_from: {0} , recv_data: {1}".format(revc_data[1], revc_data[0]))
        if(revc_data[0].decode() == 'EOF'):
            break

        Tuple = String2Tuple(revc_data[0].decode())
        sketch[Tuple[1]] = Tuple[0]

    for index in range(0, buck_num):
        if(sketch[index] == -1.0):
            # lost positions
            sketch[index] = default_value

    udp_socket.close()
    print(sketch)

if __name__ == "__main__":
    main()
