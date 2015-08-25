
kgi_plugin_fix_svr: fix_tcp_svr.cpp jsoncpp.cpp Application.h Application.cpp
	c++ -g -I /usr/local/boost/include/ -I /usr/local/quickfix/include/ -I/usr/local/redisclient/include/  fix_tcp_svr.cpp jsoncpp.cpp Application.h Application.cpp -L/usr/local/quickfix/lib/ -lquickfix -lpthread -std=c++0x  -o kgi_plugin_fix_svr /usr/local/boost/lib/libboost_system.a 


trclient: Application.h Application.cpp tradeclient.cpp
	c++  -g -I /usr/local/quickfix/include/  Application.h Application.cpp tradeclient.cpp -L/usr/local/quickfix/lib/ -lquickfix -lpthread -std=c++0x -o trclient


clean: 
	rm -rf kgi_plugin_fix_svr
	rm -rf trclient

