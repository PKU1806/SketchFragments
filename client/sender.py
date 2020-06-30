import socket
import numpy as np
import random
import sys

sys.path.append('../')

from utils.utils import Tuple2String
from client.config import send_addr, buck_num, loss

sketch = np.random.randint(100, size=buck_num)

def main():
    # 创建udp套接字,
    # AF_INET表示ip地址的类型是ipv4，
    # SOCK_DGRAM表示传输的协议类型是udp
    udp_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    print(sketch)
    # 要发送的ip地址和端口（元组的形式）
    print('send_addr = ', send_addr)

    sketch_fragments = []
    index = 0
    for buck in sketch:
        sketch_fragments.append([buck, index])
        index += 1

    random.shuffle(sketch_fragments)

    count = 0

    for buck, index in sketch_fragments:
        if(count >= buck_num * (1.0 - loss)):
            break
        count += 1
        sent_data = Tuple2String((buck, index))
        print('send_data: ', sent_data)
        udp_socket.sendto(sent_data.encode(), send_addr)

    sent_data = 'EOF'
    print('send_data: ', sent_data)
    udp_socket.sendto(sent_data.encode(), send_addr)

    udp_socket.close()


if __name__ == "__main__":
    main()
