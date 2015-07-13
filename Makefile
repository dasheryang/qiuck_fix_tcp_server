

server: fix_tcp_svr.cpp jsoncpp.cpp Application.h Application.cpp
	c++ -g -I /usr/local/boost/include/ -I /usr/local/quickfix/include/  fix_tcp_svr.cpp jsoncpp.cpp Application.h Application.cpp -L/usr/local/quickfix/lib/ -lquickfix -lpthread -std=c++0x  -o server /usr/local/boost/lib/libboost_system.a 

trclient: Application.h Application.cpp tradeclient.cpp
	c++  -g -I /usr/local/quickfix/include/  Application.h Application.cpp tradeclient.cpp -L/usr/local/quickfix/lib/ -lquickfix -lpthread -std=c++0x -o trclient


clean: 
	rm -rf server
	rm -rf trclient

