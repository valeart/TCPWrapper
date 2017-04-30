#ifndef TCPWRAPPER_CLIENT_CLASS
#define TCPWRAPPER_CLIENT_CLASS

#include "CommonTypes.h"
#include "Connection.h"
#include <boost/asio.hpp>
#include <string>

namespace TcpWrapper{
	
	class Client{
		const Connection connection;
		const PacketProtocol* protocol;

		boost::asio::io_service conn_io_service;
		boost::asio::ip::tcp::socket conn_socket;

		void Connect();
		void PrintError(std::string error) const;

	public:
		Client(IpAddrType ip_addr, int port, const PacketProtocol* prot):
		  connection(ip_addr, port), protocol(prot), conn_socket(conn_io_service){Connect();}
		Client(const Connection& conn, const PacketProtocol* prot):
		  connection(conn), protocol(prot), conn_socket(conn_io_service){Connect();}

		std::string getResponse(const std::string& request);
		virtual ~Client(){conn_socket.close();}
	};

}

#endif //TCPWRAPPER_CLIENT_CLASS