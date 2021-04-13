//
// Created by pmerku on 30/03/2021.
//

#include "server/http/CGIClass.hpp"
#include "env/ENVBuilder.hpp"
#include "utils/intToString.hpp"
#include "server/http/HTTPParser.hpp"
#include <signal.h>
#include <algorithm>

using namespace NotApache;

CgiClass::CgiClass() : _envp(), pid(), status(0), response(HTTPParseData::CGI_RESPONSE), hasExited(true) {}

CgiClass::~CgiClass() {
	if (!hasExited) {
		::kill(pid, SIGKILL);
	}
}

void CgiClass::generateENV(HTTPClient& client, const utils::Uri& uri, const std::string &rewrittenUrl) {
	HTTPParseData data = client.data.request.data;
    std::string	domain = (*data.headers.find("HOST")).second;
    domain = domain.substr(0, domain.find(':'));

    CGIenv::ENVBuilder builder;
	builder.SERVER_NAME(domain); // domain name from host header

	// content length
	if (data.isChunked)
		builder.CONTENT_LENGTH(utils::intToString(data.chunkedData.size()));
	else
        builder.CONTENT_LENGTH(utils::intToString(data.data.size()));

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

	for (std::map<std::string, std::string>::iterator it = data.headers.begin(); it != data.headers.end(); ++it) {
		std::string key = "HTTP_";
		key += it->first;
		std::replace(key.begin(), key.end(), '-', '_');
		builder.EXPORT(key, it->second);
	}

	std::map<std::string, std::string>::iterator it = data.headers.find("AUTHORIZATION");
    if (it != data.headers.end())
        builder.AUTH_TYPE(it->second);
    it = data.headers.find("CONTENT_TYPE");
    if (it != data.headers.end())
        builder.CONTENT_TYPE(it->second);
    it = data.headers.find("REMOTE_USER");
    if (it != data.headers.end())
        builder.REMOTE_USER(it->second);

    _envp.setEnv(builder.build());
}

CGIenv::env &CgiClass::getEnvp() {
	return _envp;
}
