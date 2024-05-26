server:
	g++ src/ChatServer.cpp src/components/ChatClient.cpp -o ChatServer.exe -lws2_32
	./ChatServer.exe