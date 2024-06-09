server:
	g++ src/server.cpp src/components/Client.cpp -o EspyntarServer.exe -lws2_32 -lraylib
	./EspyntarServer.exe