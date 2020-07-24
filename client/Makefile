all : sender receiver traffic


traffic : traffic.cpp sender receiver
	g++ traffic.cpp -o traffic -O2 --std=c++11 -lpthread

sender : sender.cpp sender.h
	g++ sender.cpp -o sender -O2 --std=c++11

receiver : receiver.cpp receiver.h
	g++ receiver.cpp -o receiver -O2 --std=c++11

clean :
	rm sender receiver
