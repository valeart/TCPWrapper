#ifndef TCPWRAPPER_OBSERVER_SERVER_CLASS
#define TCPWRAPPER_OBSERVER_SERVER_CLASS

#include "CommonServer.h"

namespace TcpWrapper{

	class Observer;

	namespace Server{

		class ObserverServer : public CommonServer{
			Observer* const observer;

		public:
			ObserverServer(IpAddrType ip_addr, int port, const PacketProtocol* prot, const ProtocolProcessor* protProc, Observer* _observer, int max_cn = -1):
			  CommonServer(ip_addr, port, prot, protProc, max_cn), observer(_observer){}
			ObserverServer(const Connection& conn, const PacketProtocol* prot, const ProtocolProcessor* protProc, Observer* _observer, int max_cn = -1):
			  CommonServer(conn, prot, protProc, max_cn), observer(_observer){}

			virtual std::string getResponse(std::string request);

			virtual ~ObserverServer(){StopImmediately();}
		};

	}
}

#endif //TCPWRAPPER_OBSERVER_SERVER_CLASS