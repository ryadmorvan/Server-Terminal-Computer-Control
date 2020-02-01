#include <WS2tcpip.h> //That's the Winsocket header we get for networking same as in DBO source files
#include <deque>  //Its a container like array or vector
#include <iostream>
#include "Serversock.h"

#pragma comment(lib, "ws2_32.lib") //We add the library using pragma comment








int main()
{ //( ͡° ͜ʖ ͡°)
	{
		Serversock server;
		if (server.init())
		{
			server.run();
		}
	}

	std::cout << "Finished running" << std::endl;

	system("pause");
	return 0;
}
