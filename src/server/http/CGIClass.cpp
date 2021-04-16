//
// Created by pmerku on 30/03/2021.
//

#include "server/http/CGIClass.hpp"
#include "env/ENVBuilder.hpp"
#include "utils/intToString.hpp"
#include "server/http/HTTPParser.hpp"
#include <signal.h>
#include <unistd.h>
#include <algorithm>

using namespace NotApache;

CgiClass::CgiClass() :
	_envp(),
	pipefd(),
	bodyPipefd(),
	body(false),
	pid(),
	status(0),
	response(HTTPParseData::CGI_RESPONSE), hasExited(true)
{
	args = new char *[3]();
	pipefd[0] = -1;
	pipefd[1] = -1;
	bodyPipefd[0] = -1;
	bodyPipefd[1] = -1;
}

CgiClass::~CgiClass() {
	if (!hasExited) {
		::kill(pid, SIGKILL);
	}
	freeArgs();
}

void CgiClass::generateENV(HTTPClient& client, const utils::Uri& uri, const std::string &rewrittenUrl) {
    std::string	domain = (*client.data.request.data.headers.find("HOST")).second;
    domain = domain.substr(0, domain.find(':'));

    CGIenv::ENVBuilder builder;
	builder.SERVER_NAME(domain); // domain name from host header

	// content length
	if (client.data.request.data.isChunked)
		builder.CONTENT_LENGTH(utils::intToString(client.data.request.data.chunkedData.size()));
	else
        builder.CONTENT_LENGTH(utils::intToString(client.data.request.data.data.size()));

	builder
		.GATEWAY_INTERFACE("CGI/1.1") // which gateway version
        .PATH_INFO(uri.path)
		.PATH_TRANSLATED("")
		.DOCUMENT_ROOT("")
        .QUERY_STRING(uri.query)
        .REMOTE_ADDR(client.getIp())
        .REMOTE_IDENT("")
        .REQUEST_METHOD(HTTPParser::methodMap_EtoS.find(client.data.request.data.method)->second)
        .REQUEST_URI(uri.getFull())
        .SCRIPT_NAME(rewrittenUrl)
	    .EXPORT("SCRIPT_FILENAME", rewrittenUrl.substr(1))
        .SERVER_PORT(utils::intToString(client.getPort()))
        .SERVER_PROTOCOL("HTTP/1.1")
        .SERVER_SOFTWARE("Not-Apache")
	    .AUTH_TYPE("")
	    .CONTENT_TYPE("")
	    .REMOTE_USER("")
	    .REDIRECT_STATUS("200");

	for (std::map<std::string, std::string>::iterator it = client.data.request.data.headers.begin(); it != client.data.request.data.headers.end(); ++it) {
		std::string key = "HTTP_";
		key += it->first;
		std::replace(key.begin(), key.end(), '-', '_');
		builder.EXPORT(key, it->second);
	}

	std::map<std::string, std::string>::iterator it = client.data.request.data.headers.find("AUTHORIZATION");
	std::map<std::string, std::string>::iterator end = client.data.request.data.headers.end();
    if (it != end)
        builder.AUTH_TYPE(it->second);
    it = client.data.request.data.headers.find("CONTENT_TYPE");
    if (it != end)
        builder.CONTENT_TYPE(it->second);
    it = client.data.request.data.headers.find("REMOTE_USER");
    if (it != end)
        builder.REMOTE_USER(it->second);

    _envp.setEnv(builder.build());
}

CGIenv::env &CgiClass::getEnvp() {
	return _envp;
}

void CgiClass::closePipes(FD *pipefd0, FD *pipefd1, FD *bodyPipefd0, FD *bodyPipefd1) {
	bool closeFail = false;
	if (pipefd0) {
		if (::close(*pipefd0) == -1)
			closeFail = true;
	}
	if (pipefd1) {
		if (::close(*pipefd1) == -1)
			closeFail = true;
	}
	if (bodyPipefd0) {
		if (::close(*bodyPipefd0) == -1)
			closeFail = true;
	}
	if (bodyPipefd1) {
		if (::close(*bodyPipefd1) == -1)
			closeFail = true;
	}
	if (closeFail)
		ERROR_THROW(CgiClass::CloseFail());
}

void CgiClass::freeArgs() {
	if (args) {
		delete [] args[0];
		delete [] args[1];
		delete [] args;
		args = 0;
	}
}
