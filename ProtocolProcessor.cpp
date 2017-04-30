#include "ProtocolProcessor.h"
#include "CommonServer.h"
#include <string>
#include <boost/thread/thread.hpp>
#include <ostream>

using namespace TcpWrapper::Server;

void ProtocolProcessor::operator()(CommonServer* server, p_socket p_conn_socket)
{
	std::string error_msg = "";
	try{
		std::string response = "";
		do{
			boost::this_thread::interruption_point();
			std::string request = server->getPacketProtocol()->ReceiveAndDecode(*p_conn_socket);
			response = server->getResponse(request);
			boost::this_thread::interruption_point();
			if(response.length() > 0)
				server->getPacketProtocol()->EncodeAndSend(response, *p_conn_socket);
		}while(response.length() > 0);
	}
	catch(boost::thread_interrupted)
	{
		;
	}
	catch(boost::system::system_error sys_error)
	{
		error_msg = "System error '" + std::string(sys_error.what()) + "' occurred.";
	}
	catch(PacketProtocol::PacketProtocolError& protocol_error)
	{
		error_msg = protocol_error.getErrorInfo();
	}

	if(error_msg != "")
	{
		error_msg += " Connection terminated\n";
		server->PrintError(error_msg);

	}

	delete p_conn_socket;
	finished = true;
}