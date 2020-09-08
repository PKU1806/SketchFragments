import os
import json

from parameter import *

global_idf = {}
sw_flows = {}

'''
input the map of switch name (switch -> client)
'''
def read_swmap():
	
	swmap = {}
	with open(swmap_file, 'r') as f:
		for line in f:
			data = line.split()
			swmap[ data[1] ] = data[0] # example: swmap['s1'] = 9
	print(swmap)
	return swmap

'''
handle the output switch logs and generate traces
'''
def handle_swlog():

	for i in range(1, num_switch+1):
		sname = 's' + str(i)
		swlog_file = swlog_dir + sname + '.log'
		trace = []
		flows = {}

		with open(swlog_file, 'r') as f:
			flag = True
			cnt = 0

			for line in f:
				if flag:
					flag = False
					continue

				# preprocess
				# line = line.replace('"True', 'True')
				# line = line.replace('"False', 'False')
				# line = line.replace('"packet_info', '"packet_info"')
				line = line.replace('timestamp":', 'timestamp":"')
				data = eval(line)
				pkt = data['packet_info']
				# print(data)

				# cnt += 1
				# if cnt >= 10:
				# 	break

				if pkt['using sketch'] == '1' :
					break

				identifier = ','.join([pkt['srcAddr'], pkt['dstAddr'], pkt['srcPort'], pkt['dstPort'], pkt['protocol']])
				# print(identifier)
				trace.append(identifier)

				if identifier not in flows:
					flows[identifier] = 0
					global_idf[identifier] = True
				flows[identifier] += 1

		# trace_name = sname + '_' + '10k' + '.dat'
		# with open(trace_dir + trace_name, 'w') as f:
		# 	for item in trace:
		# 		f.write(item+'\n')

		sw_flows[sname] = flows

'''
check the sum of download sketches
'''
def sum_dlsk():

	for i in range(1, num_switch+1):
		sname = 's' + str(i)
		dlsk_name = 'switch' + swmap[sname] + '_array0.txt'
		dlsk_dir = './download_sketch/'

		res = 0
		with open(dlsk_dir + dlsk_name, 'r') as f:
			for line in f:
				data = line.strip().split()
				for item in data:
					res += int(item)
		print(res)


'''
print routing path for a specific flow
'''
def print_path(identifier):
	# srcAddr = '10.15.5.2'
	# dstAddr = '10.14.3.2'
	# srcPort = '58667'
	# dstPort = '19783'
	# protocol = '17'
	# identifier = ','.join([srcAddr, dstAddr, srcPort, dstPort, protocol])
	# identifier = '10.13.2.2,10.20.15.2,51486,20205,17'

	exist_sw = []
	for i in range(1, num_switch+1):
		sname = 's' + str(i)
		if identifier in sw_flows[sname]:
			exist_sw.append(sname)

	print(exist_sw)
	return exist_sw

'''
check switch log for each flow, show the drop result 
'''
def check_drop():
	overall = True
	for identifier, _ in global_idf.items():
		print(identifier)
		exist_sw = print_path(identifier)
		cnt = [(sw_flows[sw_name][identifier], sw_name) for sw_name in exist_sw]
		print(cnt)
		flag = True
		for item in cnt:
			if item[0] != cnt[0][0]:
				flag = False
				break
		overall |= flag
		print(flag)
		print('')
	print('overall ' + str(overall))

if __name__ == '__main__':
	swmap = read_swmap()
	handle_swlog()
	# print_path()
	check_drop()
	# sum_dlsk()
