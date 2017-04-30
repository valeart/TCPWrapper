#include "Client.h"
#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <iostream>

void TcpWrapper::Client::Connect()
{
	conn_socket.close();

	using namespace boost::asio::ip;
	tcp::endpoint endpoint(address::from_string(connection.getIp()), connection.getPort());
	conn_socket.connect(endpoint);
}

std::string TcpWrapper::Client::getResponse(const std::string &request)
{
	std::string error_msg = "";
	try{
		if(!conn_socket.is_open())
			Connect();
	
		protocol->EncodeAndSend(request, conn_socket);
		return protocol->ReceiveAndDecode(conn_socket);
	}
		catch(boost::system::system_error sys_error)
	{
		error_msg = "System error '" + std::string(sys_error.what()) + "' occurred.";
	}
	catch(PacketProtocol::PacketProtocolError& protocol_error)
	{
		error_msg = protocol_error.getErrorInfo();
		
	}

	PrintError(error_msg);
	return "";
}

void TcpWrapper::Client::PrintError(std::string error) const
{
	std::cerr << error;
}