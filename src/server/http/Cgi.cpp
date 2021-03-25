#include "server/http/Cgi.hpp"

void	cgi::setMetaVars(HTTPClientRequest& request) {
	_metaVars["AUTH_TYPE"] = request._headers["AUTHORIZATION"];
	_metaVars["CONTENT-LENGTH"] = utils::itos(request._body.length());
	_metaVars["CONTENT-TYPE"] = request._headers["CONTENT-TYPE"];
	_metaVars["GATEWAY-INTERFACE"] = "CGI/1.1";
	// _metaVars["PATH-INFO"] =
	// _metaVars["PATH-TRANSLATED"] =
	// _metaVars["QUERY-STRING"] =
	// _metaVars["REMOTE-ADDR"] =
	// _metaVars["REMOTE-IDENT"] =
	_metaVars["REMOTE-USER"] = request._headers["REMOTE-USER"];
	_metaVars["REQUEST-METHOD"] = HTTPParser::e_methodMap.find(request._method)->second;
	_metaVars["REQUEST-URI"] = request._uri;
	// _metaVars["SCRIPT-NAME"] = 
	// _metaVars["SERVER-NAME"] = 
	// _metaVars["SERVER-PORT"] = 
	_metaVars["SERVER-PROTOCOL"] = "HTTP/1.1";
	_metaVars["SERVER-SOFTWARE"] = "HTTP 1.1";
}

void	cgi::setEnv() {
	_env = (char**)malloc(sizeof(char*) * (_metaVars.size() + 1));
	if (!_env)
		return ; // error

	std::map<std::string, std::string>::iterator it = _metaVars.begin();
	for (size_t i = 0; it != _metaVars.end(); ++it) {
		std::string str = it->first + "=" + it->second;
		_env[i] = utils::strdup(str.c_str());
		if (!_env[i])
			return ; // error
		++i;
	}
}

void	cgi::runCGI(HTTPClientRequest& request, const std::string& path) {
	int pid;
	char** args;

	setMetaVars(request);
	setEnv();

	pid = fork();
	if (pid < 0)
		return ; // error
	if (!pid)
	{
		if (execve(args[0], args, _env) < 0)
			return ; // error
	}
}
