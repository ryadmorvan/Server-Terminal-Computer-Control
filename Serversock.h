#pragma once
#include <WS2tcpip.h>
#include <string>
#include <map>
#include <thread>
#pragma comment(lib, "ws2_32.lib")


class Serversock
{
public:

	Serversock() = default;

	bool		init();

	~Serversock();
	void		run();
	void		clean();

private:
	SOCKET		CreateSocket();
	bool	    bindSocket();
	bool		ConnectionAccept();
private:
	unsigned m_ID{ 0 };
	WSADATA m_wsData;
	std::string m_ipAddress = "";
	std::thread m_thread;
	SOCKET m_listening;
	std::map<int, std::string> hosts;
	fd_set master;
};

