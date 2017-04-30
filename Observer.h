#ifndef TCPWRAPPER_OBSERVER_CLASS
#define TCPWRAPPER_OBSERVER_CLASS

#include "CommonServer.h"
#include <string>

namespace TcpWrapper{

	class Observer{

	public:
		virtual std::string processRequest(std::string request) = 0;
		
	};

}

#endif //TCPWRAPPER_OBSERVER_CLASS