#ifndef TCPWRAPPER_SERVER_COMMON_SERVER_CLASS
#define TCPWRAPPER_SERVER_COMMON_SERVER_CLASS

#include "CommonTypes.h"
#include "Connection.h"
#include <boost/thread.hpp>
#include <list>

namespace TcpWrapper{

	namespace Server{

		class ListenerThread;
		class ProtocolProcessor;

		class CommonServer{
			Connection connection;
			const TcpWrapper::PacketProtocol* protocol;
			boost::asio::io_service conn_io_service;
			boost::asio::ip::tcp::acceptor* p_acceptor;
			
			static const int def_max_conn;

			friend class ListenerThread;
			boost::thread* listener_thread;

			const ProtocolProcessor* const protocolProcessor;
			
			typedef std::pair<boost::thread*, ProtocolProcessor*> requestProcessorType;
			typedef std::list<requestProcessorType> requestProcessorsListType;
			requestProcessorsListType processing_threads;
			friend bool checkActiveThread(requestProcessorType& processing_thread);

			//std::list<boost::thread*> processing_threads;
			//std::list<ProtocolProcessor*> processing_thread_objs;
			static boost::mutex running_mutex;

			CommonServer(const CommonServer&);
			CommonServer& operator=(const CommonServer&);

			void Accept();
			boost::asio::ip::tcp::socket* getNextConnection();
			void ConnectToItself();
			void StopAcceptor();

		protected:
			int max_conn;
			int conn_amount;
			bool running;
			bool immediately;

			bool freeNonworkingThreads(bool force = false);
			void PrintInformation(std::string info) const;
			
		public:
			CommonServer(IpAddrType ip_addr, int port, const PacketProtocol* prot, const ProtocolProcessor* protProc, int max_cn = -1);
			CommonServer(const Connection& conn, const PacketProtocol* prot, const ProtocolProcessor* protProc, int max_cn = -1);

			void Start();
			void StopSmoothly();
			void StopImmediately();

			bool connAvailable() const {return conn_amount < max_conn;}
			bool isRunning() const {return running;}

			inline const PacketProtocol* getPacketProtocol() const {return protocol;}
			virtual std::string getResponse(std::string request) = 0;

			void PrintError(std::string error) const;

			virtual ~CommonServer(){StopImmediately();}
		};
	}
}

#endif //TCPWRAPPER_SERVER_COMMON_SERVER_CLASS