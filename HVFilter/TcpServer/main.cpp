#include <WinSock2.h>
#include <iostream>

int main(int argc, const char* argv[])
{
	std::string sIP;
	int port = 0;

	if (argc < 2) {
		port = 9000;
	} else {
		port = atoi(argv[1]);
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

	SOCKADDR_IN InternetAddr;
	InternetAddr.sin_family = AF_INET;
	InternetAddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	InternetAddr.sin_port = htons(port);

	std::cout << "binding on port " << port << std::endl;

	if (SOCKET_ERROR ==  bind(s, (sockaddr*)&InternetAddr, sizeof(InternetAddr)))
	{
		std::cout << "socket error: " << WSAGetLastError() << std::endl;
		return WSAGetLastError();
	}

	/*int time = 5000;
	int nError = setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, (const char*)&time, sizeof(time));
	if (nError != 0)
	{
		std::cout << "socket error: " << WSAGetLastError() << std::endl;
		return WSAGetLastError();
	}*/

	std::cout << "bound on port=" << port << std::endl;
	std::cout << "listening..." << std::endl;

	if (SOCKET_ERROR == listen(s, 1))
	{
		std::cout << "socket error: " << WSAGetLastError() << std::endl;
		return WSAGetLastError();
	}

	std::cout << "bound finished. accepting connection" << std::endl;

	SOCKADDR_IN ClientAddr;
	int dSize = sizeof(SOCKADDR_IN);
	SOCKET conn = accept(s, (SOCKADDR*)&ClientAddr, &dSize);
	if (conn == INVALID_SOCKET)
	{
		std::cout << "socket error: " << WSAGetLastError() << std::endl;
		return WSAGetLastError();
	}

	/*int time = 5000;
	int nError = setsockopt(conn, SOL_SOCKET, SO_RCVTIMEO, (const char*)&time, sizeof(time));
	if (nError != 0)
	{
		return WSAGetLastError();
	}*/

	char buf[9];

	std::cout << "receiving bytes" << std::endl;
	result = recv(conn, buf, 9, 0);
	if (SOCKET_ERROR == result) {
		std::cout << "socket error: " << WSAGetLastError() << std::endl;
		return WSAGetLastError();
	}

	std::cout << "receive finished. sending message back..." << std::endl;

	result = send(conn, "what", 4, 0);
	if (SOCKET_ERROR == result) {
		std::cout << "socket error: " << WSAGetLastError() << std::endl;
		return WSAGetLastError();
	}

	result = send(conn, "ever", 5, 0);
	if (SOCKET_ERROR == result) {
		std::cout << "socket error: " << WSAGetLastError() << std::endl;
		return WSAGetLastError();
	}

	std::cout << "message sent." << std::endl;
	while (true) {
		Sleep(1000);
	}

	/*Sleep(1000);

	std::cout << "awaken. shutting down..." << std::endl;
	result = shutdown(s, SD_BOTH);
	if (result) {
		std::cout << "socket error: " << result << " = " << WSAGetLastError() << std::endl;
		return WSAGetLastError();
	}

	std::cout << "shut down finished. closing conn..." << std::endl;

	if (!closesocket(conn)) {
		std::cout << "socket error: " << result << " = " << WSAGetLastError() << std::endl;
		return WSAGetLastError();
	}

	std::cout << "conn closed. closing server socket..." << std::endl;

	if (!closesocket(s)) {
		std::cout << "socket error: " << result << " = " << WSAGetLastError() << std::endl;
		return WSAGetLastError();
	}

	std::cout << "closing finished. cleaning up..." << std::endl;

	if (WSACleanup() != 0)
	{
		std::cout << "socket error: " << WSAGetLastError() << std::endl;
		return WSAGetLastError();
	}

	std::cout << "cleanup finished!" << std::endl;*/

	return 0;
}