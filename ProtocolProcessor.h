#ifndef TCPWRAPPER_PROTOCOL_PROCESSOR_CLASS
#define TCPWRAPPER_PROTOCOL_PROCESSOR_CLASS

#include "CommonTypes.h"

namespace TcpWrapper{

	namespace Server{

			class CommonServer;

			class ProtocolProcessor
			{
			protected:
				typedef boost::asio::ip::tcp::socket* p_socket;
				void setInactive(){finished = true;}

			private:
				bool finished;

			public:
				ProtocolProcessor():
				   finished(false){}

				virtual void operator()(CommonServer* server, p_socket p_conn_socket);
				virtual ProtocolProcessor* clone() const {return new ProtocolProcessor;}

				bool isActive() const {return !finished;}

				virtual ~ProtocolProcessor(){}
			};

	}
}

#endif //TCPWRAPPER_PROTOCOL_PROCESSOR_CLASS