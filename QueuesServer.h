#ifndef TCPWRAPPER_QUEUES_SERVER_CLASS
#define TCPWRAPPER_QUEUES_SERVER_CLASS

namespace TcpWrapper{

	namespace Server{

		class QueuesServer: private CommonServer{

			virtual ProtocolProcessor* getProtocolProcessor();

		public:
			QueuesServer(IpAddrType ip_addr, int port, const Protocol* prot, const Observer* _observer, int max_cn = -1):
			  CommonServer(ip_addr, port, prot, max_cn){}

			virtual ~QueuesServer(){StopImmediately();}
		};

	}
}

#endif //TCPWRAPPER_QUEUES_SERVER_CLASS