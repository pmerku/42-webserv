//
// Created by pmerku on 30/03/2021.
//

#include "server/http/CGIClass.hpp"
#include "env/ENVBuilder.hpp"
#include "utils/intToString.hpp"
#include "server/http/HTTPParser.hpp"
#include <signal.h>

using namespace NotApache;

CgiClass::CgiClass() : _envp(), pid(), status(0), response(HTTPParseData::CGI_RESPONSE), hasExited(true) {}

CgiClass::~CgiClass() {
	if (!hasExited) {
		::kill(pid, SIGKILL);
	}
}

void CgiClass::generateENV(HTTPClient& client, const utils::Uri& uri, const std::string &filePath, const std::string &execPath) {
    std::string	domain = (*client.data.request.data.headers.find("HOST")).second;
    domain = domain.substr(0, domain.find(':'));

    CGIenv::ENVBuilder env;
	env.SERVER_NAME(domain)
        .CONTENT_LENGTH(utils::intToString(client.data.request.data.data.size())) // TODO check with chunked
        .GATEWAY_INTERFACE("CGI/1.1")
        .PATH_INFO(uri.path) // TODO URL translating/encoding
        .PATH_TRANSLATED(filePath)
        .QUERY_STRING(uri.query)
        .REMOTE_ADDR(client.getIp())
        .REMOTE_IDENT("") // TODO what is this? (maybe not handle)
        .REQUEST_METHOD(HTTPParser::methodMap_EtoS.find(client.data.request.data.method)->second)
        .REQUEST_URI(uri.getFull())
        .SCRIPT_NAME(execPath)
        .SERVER_PORT(utils::intToString(client.getPort()))
        .SERVER_PROTOCOL("HTTP/1.1")
        .SERVER_SOFTWARE("Not-Apache")
	    .AUTH_TYPE("")
	    .CONTENT_TYPE("")
	    .REMOTE_USER("")
	    .REDIRECT_STATUS("200");

	// TODO HTTP_<headers>
    std::map<std::string, std::string>::iterator it;
    std::map<std::string, std::string>::iterator end = client.data.request.data.headers.end();
    it = client.data.request.data.headers.find("AUTHORIZATION");
    if (it != end)
        env.AUTH_TYPE(it->second);
    it = client.data.request.data.headers.find("CONTENT_TYPE");
    if (it != end)
        env.CONTENT_TYPE(it->second);
    it = client.data.request.data.headers.find("REMOTE_USER");
    if (it != end)
        env.REMOTE_USER(it->second);

    _envp.setEnv(env.build());
}

const CGIenv::env &CgiClass::getEnvp() const {
	return _envp;
}
