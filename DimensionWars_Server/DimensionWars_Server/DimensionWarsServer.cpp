#include "stdafx.h"
#include "ServerManager.h"

int main()
{
	std::unique_ptr<ServerManager> server = std::make_unique<ServerManager>();
	server->Run();
}