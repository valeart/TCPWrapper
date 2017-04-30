#ifndef TCPWRAPPER_SIMPLE_PACKET_PROTOCOL_CLASS
#define TCPWRAPPER_SIMPLE_PACKET_PROTOCOL_CLASS

#include "CommonTypes.h"
#include <boost/asio.hpp>

namespace TcpWrapper{
	
	class SimplePacketProtocol: public PacketProtocol{
		static const int Packet_header;
		static const int Packet_size;
		static const long Max_len;

		std::string encodeLen(int size) const;

	public:
		SimplePacketProtocol(){}

		virtual void EncodeAndSend(const std::string& str, boost::asio::ip::tcp::socket& socket) const;
		virtual std::string ReceiveAndDecode(boost::asio::ip::tcp::socket& socket) const;

		virtual ~SimplePacketProtocol(){}

		class BigSize : public PacketProtocolError{
			const long size;

		public:
			explicit BigSize(long s): size(s){}

			inline long getSize() const {return size;}
			virtual std::string getErrorInfo() const;
		};

		class ReadError : public PacketProtocolError{
			const std::string info;

		public:
			explicit ReadError(const char* pinfo):info(pinfo){}

			inline const std::string& getInfo() const {return info;}
			virtual std::string getErrorInfo() const;
		};

	};

}

#endif //TCPWRAPPER_SIMPLE_PACKET_PROTOCOL_CLASS