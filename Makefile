server:
	g++ src/server.cpp src/components/ChatClient.cpp -o EspyntarServer.exe -lws2_32
	./EspyntarServer.exe