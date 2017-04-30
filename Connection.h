#ifndef TCPWRAPPER_CONNECTION_CLASS
#define TCPWRAPPER_CONNECTION_CLASS

#include "CommonTypes.h"
#include <string>

namespace TcpWrapper{
	
	class Connection{
		IpAddrType ip_address;
		int port;

		mutable std::string ip_addr_str;	//string representation of Ip address

	public:
		Connection(IpAddrType ip_addr, int pr): ip_address(ip_addr), port(pr), ip_addr_str(""){}

		inline int getPort() const {return port;}
		inline const std::string& getIp() const {return ip_address.getIp();};

		virtual ~Connection(){}
	};

}

#endif //TCPWRAPPER_CONNECTION_CLASS