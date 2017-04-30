#include "ObserverServer.h"
#include "Observer.h"

using namespace TcpWrapper::Server;

std::string TcpWrapper::Server::ObserverServer::getResponse(std::string request)
{
	return observer->processRequest(request);
}

