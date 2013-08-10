#include <WinSock2.h>
#include <iostream>

int main(int argc, const char* argv[])
{
	if (argc < 3) {
		std::cout << "call: testtcpip 192.168.0.2 9000" << std::endl;
		return -1;
	}

	std::string sIP = argv[1];
	int port = atoi(argv[2]);
	if (port < 1) {
		std::cout << "port number too small!" << std::endl;
		return -1;
	}

	WSADATA wsaData;
	int result = WSAStartup(MAKEWORD(2,2), &wsaData);
	if (result != 0)
	{
		std::cout << "win error: " << GetLastError() << std::endl;

		return GetLastError();
	}

	std::cout << "finished startup" << std::endl;

	/**********************/

	SOCKET s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (s == INVALID_SOCKET)
	{
		std::cout << "socket error: " << WSAGetLastError() << std::endl;
		return WSAGetLastError();
	}

	std::cout << "created socket" << std::endl;

	int time = 5000;
	int nError = setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, (const char*)&time, sizeof(time));
	if (nError != 0)
	{
		std::cout << "socket error: " << WSAGetLastError() << std::endl;
		return WSAGetLastError();
	}

	std::cout << "set socket blocking" << std::endl;

	//173.194.32.46 80
	SOCKADDR_IN server_address;
	server_address.sin_family = AF_INET;
	server_address.sin_addr.S_un.S_addr = inet_addr(sIP.data());
	server_address.sin_port = htons(port);

	//client
	if (0 != connect(s, (SOCKADDR*)&server_address, sizeof(server_address)))
	{
		std::cout << "socket error: " << WSAGetLastError() << std::endl;
		return WSAGetLastError();
	}

	std::cout << "connected to: ip=" <<  sIP.data() << "; port=" << port << std::endl;

	result = send(s, "mata este proasta!", 19, 0);
	if (SOCKET_ERROR == result) {
		std::cout << "socket error: " << WSAGetLastError() << std::endl;
		return WSAGetLastError();
	}

	std::cout << "sent message" << std::endl;

	result = shutdown(s, SD_BOTH);
	if (result) {
		std::cout << "socket error: " << result << " = " << WSAGetLastError() << std::endl;
		return WSAGetLastError();
	}

	std::cout << "shut down" << std::endl;

	if (WSACleanup() != 0)
	{
		std::cout << "socket error: " << WSAGetLastError() << std::endl;
		return WSAGetLastError();
	}

	std::cout << "cleanup!" << std::endl;

	return 0;
}