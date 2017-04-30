#ifndef TCPWRAPPER_LISTENER_THREAD_CLASS
#define TCPWRAPPER_LISTENER_THREAD_CLASS

#include "CommonTypes.h"

namespace TcpWrapper{

	namespace Server{

		class CommonServer;

		class ListenerThread
		{
		public:

			void operator()(CommonServer* server);
		};
	}
}

#endif //TCPWRAPPER_LISTENER_THREAD_CLASS