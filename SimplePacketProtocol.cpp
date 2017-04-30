#include "SimplePacketProtocol.h"
#include <boost/array.hpp>
#include <vector>
#include <ostream>

const int TcpWrapper::SimplePacketProtocol::Packet_header = 4;
const int TcpWrapper::SimplePacketProtocol::Packet_size = 1024;
const long TcpWrapper::SimplePacketProtocol::Max_len = 1024*1024;

std::string TcpWrapper::SimplePacketProtocol::encodeLen(int size) const
{
	std::ostringstream result_stream;
	result_stream << size;
	std::string result = result_stream.str();
	result = std::string(Packet_header - result.length(), '0') + result;
	return result;
}

void TcpWrapper::SimplePacketProtocol::EncodeAndSend(const std::string& str, boost::asio::ip::tcp::socket& socket) const
{
	long message_len = str.length();
	if(message_len > Max_len)
		throw BigSize(message_len);

	int packets_amount = (message_len-1)/Packet_size + 1;
	for(int i = 0; i < packets_amount; i++){	//write packet by packet
		std::string result;
		int str_len;

		if(i == packets_amount - 1){				
			str_len = message_len - i*Packet_size;		//calculate size of last packet
			result = encodeLen(str_len) + str.substr(i*Packet_size, str_len);
		}else{
			str_len = Packet_size + 1;					//mark that this is not the last packet
			result = encodeLen(str_len) + str.substr(i*Packet_size, Packet_size);
		}

		boost::system::error_code error;
		socket.write_some(boost::asio::buffer(result), error);
	} //for(i)
}

std::string TcpWrapper::SimplePacketProtocol::ReceiveAndDecode(boost::asio::ip::tcp::socket& socket) const
{
	boost::array<char, Packet_header> buffer_len;
	std::string buffer_info;
	boost::system::error_code error;

	std::string result = "";
	while(true){																		//read forever (until last segment)
		size_t read_len = socket.read_some(boost::asio::buffer(buffer_len), error);		//read length of information segment
		if(error)
			throw boost::system::system_error(error);
		if(read_len != Packet_header)													//wrong size of length segment
			throw ReadError(buffer_len.c_array());

		int info_len = atoi(buffer_len.c_array());
		if(info_len == 0 || info_len > Packet_size+1)
			throw ReadError(buffer_len.c_array());

		info_len > Packet_size ? buffer_info.resize(Packet_size) : buffer_info.resize(info_len);
		read_len = socket.read_some(boost::asio::buffer(buffer_info), error);			//read information segment
		if(error)
			throw boost::system::system_error(error);
		if(read_len != buffer_info.length())											//wrong size of information segment
			throw ReadError(buffer_info.c_str());

		result += buffer_info;															//add information
		if(info_len <= Packet_size)
			break;

	}

	return result;
}

std::string TcpWrapper::SimplePacketProtocol::BigSize::getErrorInfo() const
{
	std::ostringstream error_stream;
	error_stream << "Big size of message (" << size << ") occurred.";
	return error_stream.str();
}

std::string TcpWrapper::SimplePacketProtocol::ReadError::getErrorInfo() const
{
	return "Reading error '" + info + "' occurred.";
}