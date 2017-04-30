#include "CommonTypes.h"
#include <istream>

TcpWrapper::IpAddrType::IpAddrType(int value1, int value2, int value3, int value4):
	ip_addr_str("")
{
	IpValues[0] = value1;
	IpValues[1] = value2;
	IpValues[2] = value3;
	IpValues[3] = value4;
}

TcpWrapper::IpAddrType::IpAddrType(int* values):
	ip_addr_str("")
{
	for(int i = 0; i < 4; i++)
		IpValues[i] = values[i];
}

TcpWrapper::IpAddrType::IpAddrType(const std::string& str)
{
	std::istringstream ip_addr_stream(str);
	for(int i = 0; i < 4; i++)
	{
		char ch;
		ip_addr_stream >> IpValues[i] >> ch;
		if(i < 3)
		{
			if(ch != '.')
				throw BadIp();
		}
		else
			if(!ip_addr_stream.eof())
				throw BadIp();
	}
	ip_addr_str = str;
}

const std::string& TcpWrapper::IpAddrType::getIp() const
{
	if(ip_addr_str.length())
		return ip_addr_str;

	std::ostringstream ip_addr_stream;
	ip_addr_stream << IpValues[0];
	for(int i = 1; i < 4; i++)
		ip_addr_stream << "." << IpValues[i];
	ip_addr_str = ip_addr_stream.str();
	return ip_addr_str;
}
