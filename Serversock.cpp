#include "Serversock.h"
#include <string>
#include <iostream>




void Sendm(fd_set& master, const SOCKET& listening)
{
	std::string message;
	std::string server = "Server: ";
	for (;;)
	{
		std::getline(std::cin, message);
		if (message.size() > 0)
		{
			server += message;
			for (unsigned i{ 0 }; i < master.fd_count; i++)
			{
				SOCKET outSocket = master.fd_array[i];
				if (outSocket != listening)
				{
					send(outSocket, server.c_str(), server.size() + 1, NULL);
				}
			}
		}
	}
}




bool Serversock::init()
{
	try
	{
		int wsok = WSAStartup(0x202, &m_wsData);
		if (wsok != NULL)
			throw "Error";
	}

	catch (std::string b)
	{
		if (b == "Error")
			return false;
	}
	return true;
}

SOCKET Serversock::CreateSocket()
{
	SOCKET listening = socket(AF_INET, SOCK_STREAM, NULL);
	if (listening == INVALID_SOCKET)
	{
		std::cerr << "Error cannot create a listening socket" << std::endl;
		return NULL;
	}
	return listening;
}


bool Serversock::ConnectionAccept()
{
	sockaddr_in client;
	int clientsize = sizeof(client);

	SOCKET clientSocket = accept(m_listening, (sockaddr*)& client, &clientsize); //This will accept the connection and store the infromation in the client and clientsize
	if (clientSocket != INVALID_SOCKET)
	{
		char host[NI_MAXHOST];
		char service[NI_MAXSERV];
		ZeroMemory(host, NI_MAXHOST);
		ZeroMemory(service, NI_MAXSERV);

		if (getnameinfo((sockaddr*)& client, sizeof(client), host, NI_MAXHOST, service, NI_MAXSERV, NULL) == 0)
		{
			std::cout << host << " connected to the server" << service << "\n";
			hosts.emplace( ++m_ID, host);
		}
		else
		{
			inet_ntop(AF_INET, (sockaddr*)& client, host, sizeof(host)); //will store the information of client ip to the host
			std::cout << host << " connected to the server" << ntohs(client.sin_port) << "\n";
			hosts.emplace(++m_ID, host);
		}
		FD_SET(clientSocket, &master);
	}
}
bool Serversock::bindSocket()
{
	sockaddr_in m_hint;
	unsigned long HINT_SIZE = sizeof(m_hint);
	m_hint.sin_family = AF_INET;
	m_hint.sin_port = htons(5400);
	m_hint.sin_addr.S_un.S_addr = INADDR_ANY;
	
	bind(m_listening, (sockaddr*)& m_hint, sizeof(HINT_SIZE));

	//We now need to bind the listening socket with the configuration of the m_hint
	listen(m_listening, SOMAXCONN); //That's the max connection listening socket can hold

	return true;
}
void Serversock::clean()
{
	closesocket(m_listening);
	WSACleanup();
}

Serversock::~Serversock()
{
	m_thread.join();
	WSACleanup();
}
void Serversock::run()
{

	m_listening = CreateSocket();

	 if (!Serversock::bindSocket())
	 {
		 std::cerr << "Fail to bind the SOCKET" << std::endl;
		 system("pause");
	 }


	fd_set master;
	FD_ZERO(&master);
	FD_SET(m_listening, &master);

	m_thread = std::thread(Sendm, std::ref(master), std::ref(m_listening));


	while (true)
	{
		fd_set copy = master;
		int socketCount = select(NULL, &copy, nullptr, nullptr, nullptr);
		for (short i{ 0 }; i < socketCount; i++)
		{
			SOCKET sock = copy.fd_array[i];
			if (sock == m_listening)
			{
				ConnectionAccept();
			}
			else
			{
				char *buf = new char[4096];
				ZeroMemory(buf, 4096);
				int bytesRecieved = recv(sock, buf, 4096, NULL);
				if (bytesRecieved > 0)
				{
					std::cout << std::string(buf, NULL, bytesRecieved) << "\n";
					for (unsigned i{ 0 }; i < master.fd_count; i++)
					{
						SOCKET outSocket = master.fd_array[i];
						if (outSocket != sock && outSocket != (m_listening))
						{
							send(outSocket, buf, bytesRecieved, NULL);
							delete[] buf;
						}
					}
				}
				else //If bytes are less than or = 0 it means it got disconnected
				{
					std::cout << "Client Disconnected" << "\n";
					closesocket(sock);
					FD_CLR(sock, &master); //Remove the socket from the master list
				}
			}
		}
	}


}
//back

//Welcome back
//I decided to create a class instead