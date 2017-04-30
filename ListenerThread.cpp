#include "ListenerThread.h"
#include "CommonServer.h"
#include <boost/date_time/posix_time/posix_time.hpp>

void TcpWrapper::Server::ListenerThread::operator()(CommonServer* server)
{
	while(true)
	{
		if(!server->isRunning())					//If server was stopped
			return;

		if(server->connAvailable())					//We have some non-working threads
			server->Accept();
		else
			if(server->freeNonworkingThreads())		//We can free some non-working threads
				continue;
			else									//Wait for some time
			{
				boost::posix_time::time_duration sec_period =  boost::posix_time::seconds(1);	//1 second
				boost::this_thread::sleep(sec_period);											//sleep for 1 second
			}
	}
}