#include "utils/itos.hpp"
#include "server/http/HTTPParser.hpp"
#include "server/http/HTTPClientData.hpp"
#include "utils/CreateMap.hpp"

using namespace NotApache;

class cgi {
	public:
		void		runCGI(HTTPClientRequest& request);

	private:
		void	setEnv(HTTPClientRequest& request);
		void	setMetaVars(HTTPClientRequest& request);

		std::map<std::string, std::string>	_metaVars;
		char**								_env;
};
