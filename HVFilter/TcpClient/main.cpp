#include <WinSock2.h>
#include <iostream>

int main(int argc, const char* argv[])
{
	std::string sIP;
	int port = 0;

	if (argc < 3) {
		sIP = "173.194.32.46";
		port = 80;
	} else {
		sIP = argv[1];
		port = atoi(argv[2]);
		if (port < 1) {
			std::cout << "port number too small!" << std::endl;
			return -1;
		} 
	}

	std::cout << "calling startup..." << std::endl;

	WSADATA wsaData;
	int result = WSAStartup(MAKEWORD(2,2), &wsaData);
	if (result != 0)
	{
		std::cout << "win error: " << GetLastError() << std::endl;

		return GetLastError();
	}

	std::cout << "finished startup" << std::endl;

	/**********************/

	std::cout << "creating socket..." << std::endl;

	SOCKET s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (s == INVALID_SOCKET)
	{
		std::cout << "socket error: " << WSAGetLastError() << std::endl;
		return WSAGetLastError();
	}

	std::cout << "created socket" << std::endl;

	//173.194.32.46 80
	SOCKADDR_IN server_address;
	server_address.sin_family = AF_INET;
	server_address.sin_addr.S_un.S_addr = inet_addr(sIP.data());
	server_address.sin_port = htons(port);

	std::cout << "connecting to " << sIP.data() << " port" << port << std::endl;

	//client
	if (0 != connect(s, (SOCKADDR*)&server_address, sizeof(server_address)))
	{
		std::cout << "socket error: " << WSAGetLastError() << std::endl;
		return WSAGetLastError();
	}

	std::cout << "connected to: ip=" <<  sIP.data() << "; port=" << port << std::endl;

	std::cout << "sending message..." << std::endl;

	result = send(s, "what", 4, 0);
	if (SOCKET_ERROR == result) {
		std::cout << "socket error: " << WSAGetLastError() << std::endl;
		return WSAGetLastError();
	}

	result = send(s, "ever", 5, 0);
	if (SOCKET_ERROR == result) {
		std::cout << "socket error: " << WSAGetLastError() << std::endl;
		return WSAGetLastError();
	}

	std::cout << "message sent. receiving..." << std::endl;

	char buf[9];
	result = recv(s, buf, 9, 0);
	if (SOCKET_ERROR == result) {
		std::cout << "socket error: " << WSAGetLastError() << std::endl;
		return WSAGetLastError();
	}

	std::cout << "finished receiving. sleeping..." << std::endl;

	Sleep(1000);
	std::cout << "awaken. shutting down..." << std::endl;

	result = shutdown(s, SD_BOTH);
	if (result) {
		std::cout << "socket error: " << result << " = " << WSAGetLastError() << std::endl;
		return WSAGetLastError();
	}

	std::cout << "shut down finished. cleaning up..." << std::endl;

	if (WSACleanup() != 0)
	{
		std::cout << "socket error: " << WSAGetLastError() << std::endl;
		return WSAGetLastError();
	}

	std::cout << "cleanup finished!" << std::endl;

	return 0;
}