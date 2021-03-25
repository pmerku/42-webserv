#include "utils/itos.hpp"
#include "server/http/HTTPParser.hpp"
#include "server/http/HTTPClientData.hpp"
#include "utils/CreateMap.hpp"
#include <unistd.h>
#include "utils/strdup.hpp"

using namespace NotApache;

class cgi {
	public:
		void		runCGI(HTTPClientRequest& request);

	private:
		void	setMetaVars(HTTPClientRequest& request);
		void	setEnv();

		std::map<std::string, std::string>	_metaVars;
		char**								_env;
};
