#ifndef TCPWRAPPER_COMMON_TYPES
#define TCPWRAPPER_COMMON_TYPES

#include <string>
#include <boost/asio.hpp>

namespace TcpWrapper{
	
	class IpAddrType{
		int IpValues[4];

		mutable std::string ip_addr_str;

	public:
		IpAddrType(int value1, int value2, int value3, int value4);
		IpAddrType(int* values);
		IpAddrType(const std::string& str);

		const std::string& getIp() const;

		struct BadIp{};

	};

	class PacketProtocol{

	public:

		class PacketProtocolError{
		public:
			virtual std::string getErrorInfo() const = 0;
		};

		virtual void EncodeAndSend(const std::string& str, boost::asio::ip::tcp::socket& socket) const = 0;
		virtual std::string ReceiveAndDecode(boost::asio::ip::tcp::socket& socket) const = 0;

		virtual ~PacketProtocol(){}
	};
}

#endif //TCPWRAPPER_COMMON_TYPES