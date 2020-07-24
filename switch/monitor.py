from __future__ import print_function

from p4utils.utils.topology import Topology
from p4utils.utils.sswitch_API import *

import threading
import time

BUCKET_NUM = 64
BIN_BUM = 10
ARRAY_NUM = 3

UPDATE_INTERVAL = 600

lock = threading.Lock()

class Monitor(threading.Thread):

    def __init__(self, sw_name):
        threading.Thread.__init__(self)

        self.topo = Topology(db="topology.db")
        self.sw_name = sw_name
        self.thrift_port = self.topo.get_thrift_port(sw_name)
        self.controller = SimpleSwitchAPI(self.thrift_port)



    def sketch_swap(self):

        self.controller.register_write("swap_control",0,1)
    
        present_sketch=self.controller.register_read("sketch_fg",0)
        
        if present_sketch == 0 :
            for index in range(0, BUCKET_NUM * BUN_NUM):
                # temp=self.controller.register_read("array0",index)
                # self.controller.register_write("array3",index,temp)
                # temp=self.controller.register_read("array1",index)
                # self.controller.register_write("array4",index,temp)
                # temp=self.controller.register_read("array2",index)
                # self.controller.register_write("array5",index,temp)

                self.controller.register_write("array3",index,0)
                self.controller.register_write("array4",index,0)
                self.controller.register_write("array5",index,0)
        else :
            for index in range(0, BUCKET_NUM * BIN_BUM):
                # temp=self.controller.register_read("array3",index)
                # self.controller.register_write("array0",index,temp)
                # temp=self.controller.register_read("array4",index)
                # self.controller.register_write("array1",index,temp)
                # temp=self.controller.register_read("array5",index)
                # self.controller.register_write("array2",index,temp)

                self.controller.register_write("array0",index,0)
                self.controller.register_write("array1",index,0)
                self.controller.register_write("array2",index,0)

        for index in range(0, BUCKET_NUM * ARRAY_NUM):
            self.controller.register_write("counter0", index, 0);
            self.controller.register_write("counter1", index, 0);
            self.controller.register_write("counter2", index, 0);

        self.controller.register_write("swap_control",0,0)
        self.controller.register_write("sketch_fg",0,1-present_sketch)

        lock.acquire()
        self.visor()
        lock.release()

        time.sleep(UPDATE_INTERVAL - 2)
    
    def visor(self):
        sf = self.controller.register_read("sketch_fg", 0)
        print("switch id :{}".format(self.sw_name))
        print("sketch flag: {}".format(sf))
        
        if sf == 1:
            print("array0:")
            for j in range(0, 10):
                for i in range(0, 16):
                    value = self.controller.register_read("array0", i * 10 + j)
                    print("|{:^3}|".format(value), end = "")
                print()

            print("array1:")
            for j in range(0, 10):
                for i in range(0, 16):
                    value = self.controller.register_read("array1", i * 10 + j)
                    print("|{:^3}|".format(value), end = "")
                print()

            print("array2:")
            for j in range(0, 10):
                for i in range(0, 16):
                    value = self.controller.register_read("array2", i * 10 + j)
                    print("|{:^3}|".format(value), end = "")
                print()
        else:
            print("array3:")
            for j in range(0, 10):
                for i in range(0, 16):
                    value = self.controller.register_read("array3", i * 10 + j)
                    print("|{:^3}|".format(value), end = "")
                print()

            print("array4:")
            for j in range(0, 10):
                for i in range(0, 16):
                    value = self.controller.register_read("array4", i * 10 + j)
                    print("|{:^3}|".format(value), end = "")
                print()

            print("array5:")
            for j in range(0, 10):
                for i in range(0, 16):
                    value = self.controller.register_read("array5", i * 10 + j)
                    print("|{:^3}|".format(value), end = "")
                print()

        # for i in range(0, 16):
        #     value = self.controller.register_read("timestamp_array0", i)
        #     print("|{:^16}|".format(value), end = "")
        #     value = self.controller.register_read("timestamp_array1", i)
        #     print("|{:^16}|".format(value), end = "")
        #     value = self.controller.register_read("timestamp_array2", i)
        #     print("|{:^16}|".format(value), end = "")

    def monitor(self):
        while True:
            current_time = self.controller.sswitch_client.get_time_elapsed_us() \
                    % (UPDATE_INTERVAL * 1000 * 1000)
            if current_time < 1 * 1000 * 1000:
                print(self.controller.sswitch_client.get_time_elapsed_us())
                self.sketch_swap()

    def run(self):
        print("monitor switch {}".format(self.sw_name))
        self.monitor()

if __name__ == "__main__":
    monitor_1 = Monitor("s1")
    monitor_2 = Monitor("s2")
    monitor_3 = Monitor("s3")
    monitor_4 = Monitor("s4")
    monitor_5 = Monitor("s5")
    monitor_6 = Monitor("s6")
    monitor_7 = Monitor("s7")
    monitor_8 = Monitor("s8")
    monitor_9 = Monitor("s9")
    monitor_10 = Monitor("s10")

    monitor_1.start()
    monitor_2.start()
    monitor_3.start()
    monitor_4.start()
    monitor_5.start()
    monitor_6.start()
    monitor_7.start()
    monitor_8.start()
    monitor_9.start()
    monitor_10.start()

    monitor_1.join()
    monitor_2.join()
    monitor_3.join()
    monitor_4.join()
    monitor_5.join()
    monitor_6.join()
    monitor_7.join()
    monitor_8.join()
    monitor_9.join()
    monitor_10.join()
