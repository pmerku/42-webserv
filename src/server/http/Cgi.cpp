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
	// _metaVars["REQUEST-METHOD"] =
	// std::map<e_method, std::string>::iterator it = HTTPParser::methodStoE.find(request._method);
	_metaVars["REQUEST-URI"] = request._uri;
	// _metaVars["SCRIPT-NAME"] = 
	// _metaVars["SERVER-NAME"] = 
	// _metaVars["SERVER-PORT"] = 
	_metaVars["SERVER-PROTOCOL"] = "HTTP/1.1";
	_metaVars["SERVER-SOFTWARE"] = "HTTP 1.1";
}

void	cgi::setEnv(HTTPClientRequest& request) {
	(void)_env;
	(void)request;
}

void	cgi::runCGI(HTTPClientRequest& request) {
	setMetaVars(request);
	setEnv(request);
}
