all : sender receiver


sender : sender.cpp
	g++ sender.cpp -o sender -O2 --std=c++11

receiver : receiver.cpp
	g++ receiver.cpp -o receiver -O2 --std=c++11

clean :
	rm sender receiver
