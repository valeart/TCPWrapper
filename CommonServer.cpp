#include "CommonServer.h"
#include "ListenerThread.h"
#include "ProtocolProcessor.h"
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/thread/locks.hpp>
#include <iostream>
#include <algorithm>

using namespace TcpWrapper::Server;

const int CommonServer::def_max_conn = 5;
boost::mutex CommonServer::running_mutex;

CommonServer::CommonServer(TcpWrapper::IpAddrType ip_addr, int port, const TcpWrapper::PacketProtocol* prot, const ProtocolProcessor* protProc, int max_cn /*= -1*/):
	connection(ip_addr, port), protocol(prot), protocolProcessor(protProc), max_conn(max_cn), p_acceptor(0), listener_thread(0), running(false), immediately(true)
{
	if(max_conn == -1)
		max_conn = def_max_conn;
}

CommonServer::CommonServer(const TcpWrapper::Connection& conn, const TcpWrapper::PacketProtocol* prot, const ProtocolProcessor* protProc, int max_cn /*= -1*/):
	connection(conn), protocol(prot), protocolProcessor(protProc), max_conn(max_cn), p_acceptor(0), listener_thread(0), running(false), immediately(true)
{
	if(max_conn == -1)
		max_conn = def_max_conn;
}

void CommonServer::Start()
{
	{//Block with locked mutex
		boost::lock_guard<boost::mutex> lock_mutex(running_mutex);		//Lock mutex (release mutex at return)				
		if(running)														//Somebody already started process
			return;

		running = true;													//Now we are running
		
		using namespace boost::asio::ip;
		tcp::endpoint endpoint(address::from_string(connection.getIp()), connection.getPort());
		p_acceptor = new tcp::acceptor(conn_io_service);								//Prepare acceptor
		p_acceptor->open(endpoint.protocol());
		p_acceptor->bind(endpoint);
		p_acceptor->listen();
		listener_thread =  new boost::thread(ListenerThread(), this);					//Launch listener thread
		PrintInformation("Server started\n");
	}
}

void CommonServer::StopSmoothly()
{
	{//Block with locked mutex
		boost::lock_guard<boost::mutex> lock_mutex(running_mutex);		//Lock mutex (release mutex at return)				
		if(!running)													//Somebody already started stop process
			return;

		running = false;	
		immediately = false;											//Stop process is smooth

		PrintInformation("Server is stopping... No more incomming connections available\n");

		while(processing_threads.size() != 0)																			//Delete processing threads
			freeNonworkingThreads(true);

		StopAcceptor();

		PrintInformation("Server stopped\n");
	}
}

void CommonServer::StopImmediately()
{
	{//Block with locked mutex
		boost::lock_guard<boost::mutex> lock_mutex(running_mutex);		//Lock mutex (release mutex at return)				
		if(!running && immediately)										//Somebody already started immediate stop process
			return;

		running = false;
		immediately = true;												//Stop process is immediate

		PrintInformation("Server is stopping... No more incomming connections available\n");

		for(requestProcessorsListType::iterator p = processing_threads.begin(); p != processing_threads.end(); ++p)		//Interrupt processing threads
			p->first->interrupt();

		while(processing_threads.size() != 0)																			//Delete processing threads
			freeNonworkingThreads(true);

		StopAcceptor();

		PrintInformation("Server stopped\n");
	}
}

boost::asio::ip::tcp::socket* CommonServer::getNextConnection()
{
	using namespace boost::asio::ip;

	tcp::socket* p_conn_socket = new tcp::socket(conn_io_service);			//Create new socket for the connection
	p_acceptor->accept(*p_conn_socket);										//Listen on the acceptor

	if(!running)															//Somebody already started stop process
	{
		delete p_conn_socket;
		return 0;
	}

	return p_conn_socket;
}

void CommonServer::Accept()
{
	boost::asio::ip::tcp::socket* p_conn_socket = getNextConnection();
	if(!p_conn_socket)
		return;

	boost::posix_time::time_duration sec_period = boost::posix_time::seconds(1);			//1 second
	while(!running_mutex.timed_lock(sec_period))											//Trying to lock mutex
	{																						//If we can't lock mutex now
		if(!running)																		//Check if anybody started stop process
		{
			delete p_conn_socket;
			return;
		}
	}

	{//Block with locked mutex
		boost::lock_guard<boost::mutex> lock_mutex(running_mutex, boost::adopt_lock_t());		//Adopt locked mutex (release mutex at return)				

		ProtocolProcessor* proc_thread_obj = protocolProcessor->clone();		//Create processing thread
		boost::thread* thread = new boost::thread(boost::ref(*proc_thread_obj), this, p_conn_socket);	//Launch processing thread
		processing_threads.push_back(requestProcessorType(thread, proc_thread_obj));
		conn_amount++;
	}
}

void CommonServer::ConnectToItself()
{
	using namespace boost::asio::ip;

	boost::asio::io_service self_connect_service;
	tcp::socket self_connect_socket(self_connect_service);
	
	self_connect_socket.close();
	tcp::endpoint self_connect_endpoint(address::from_string(connection.getIp()), connection.getPort());
	self_connect_socket.connect(self_connect_endpoint);
}

void CommonServer::StopAcceptor()
{
	while(listener_thread){
		ConnectToItself();																	//Connect to itself to stop accepting connections
		boost::posix_time::time_duration sec_period =  boost::posix_time::seconds(1);	//1 second
		if(listener_thread->timed_join(sec_period)){
			delete listener_thread;
			delete p_acceptor;																//Destory acceptor
			listener_thread = 0;
		}
	}
}

bool TcpWrapper::Server::checkActiveThread(std::pair<boost::thread*, ProtocolProcessor*>& processing_thread)
{
	if(!(processing_thread.second->isActive()))		//If process is finished
	{
		processing_thread.first->join();			//Terminate process
		delete processing_thread.first;
		delete processing_thread.second;
		return true;								
	}

	return false;
}

bool CommonServer::freeNonworkingThreads(bool force /*= false*/)
{
	if(!force && !running)
		return false;

	{//Block with locked mutex (if required)
		boost::unique_lock<boost::mutex> lock_mutex(running_mutex, boost::defer_lock_t());		//Adopt locked mutex (release mutex at return)
		if(!force)								//If we need to lock mutex
		{
			if(!lock_mutex.try_lock())
				return false;					//Exit if we couldn't lock mutex
		}

		unsigned old_size = processing_threads.size();

		requestProcessorsListType active_processing_threads;
		std::remove_copy_if(processing_threads.begin(), processing_threads.end(), std::back_inserter(active_processing_threads), checkActiveThread);
		processing_threads.swap(active_processing_threads);

		//std::remove_if(processing_threads.begin(), processing_threads.end(), checkInactiveThread);

		unsigned new_size = processing_threads.size();

		return processing_threads.size() < old_size;		//We could free some non-working threads
	}
}


/*		while(p != processing_threads.end())
		{
			if(!(p->second->isActive()))					//If process is finished
			{
				p->first->join();							//Terminate process
				delete p->first;
				delete p->second;
				p = processing_threads.erase(p);
				found = true;						//We can free some non-working threads
			}
			else
				p++;
		}
*/


void CommonServer::PrintError(std::string error) const
{
	std::cerr << error;
}

void CommonServer::PrintInformation(std::string info) const
{
	std::clog << info;
}
