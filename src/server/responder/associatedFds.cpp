//
// Created by jelle on 4/12/2021.
//

#include "server/http/HTTPResponder.hpp"
#include "server/global/GlobalPlugins.hpp"

using namespace NotApache;

void HTTPResponder::generateAssociatedResponse(HTTPClient &client) {
	client.clearAssociatedFd();
	// if file, build body and send
	if (client.responseState == FILE) {
		// loops through plugins and executes if plugin is loaded
		for (plugin::PluginContainer::pluginIterator it = globalPlugins.begin(); it != globalPlugins.end(); ++it) {
			if (it->second && it->first->onSendFile(client))
				return;
		}

		client.data.response.setResponse(
			client.data.response.builder
				.setBody(client.data.response.getAssociatedDataRaw())
				.build()
		);
	}
	// when done upload, build and send
	else if (client.responseState == UPLOAD) {
		client.data.response.setResponse(
			client.data.response.builder.build()
		);
	}
	// if proxy, handle errors if any or build and send
	else if (client.responseState == PROXY) {
		if (client.proxy->response.data.parseStatusCode != 200) {
			handleError(client, 502);
			return;
		}
		client.replyStatus = client.proxy->response.data.statusCode;
		client.data.response.setResponse(
			ResponseBuilder(client.proxy->response.data)
				.build()
		);
	}
	// if cgi, handle parse error if any, handle exec error if any, or build and send
	else if (client.responseState == CGI) {
		// parse error
		if (client.cgi->response.data.parseStatusCode != 200) {
			handleError(client, 500);
			return;
		}

		// child process exit codes
		if (client.cgi->status != 0) {
			if (client.cgi->status == EXECVE_ERROR)
				globalLogger.logItem(logger::ERROR, "CGI error: execve");
			else if (client.cgi->status == CLOSE_ERROR)
				globalLogger.logItem(logger::ERROR, "CGI error: close");
			else if (client.cgi->status == DUP2_ERROR)
				globalLogger.logItem(logger::ERROR, "CGI error: dup2");
			else if (client.cgi->status == CHDIR_ERROR)
				globalLogger.logItem(logger::ERROR, "CGI error: chdir");
			else if (client.cgi->status == GETCWD_ERROR)
				globalLogger.logItem(logger::ERROR, "CGI error: getcwd");
			else if (client.cgi->status == MEMORY_ERROR)
				globalLogger.logItem(logger::ERROR, "CGI error: memory");
			handleError(client, 500);
			return;
		}

		// cgi success, respond normally
		client.replyStatus = client.cgi->response.data.statusCode;
		client.data.response.setResponse(
			ResponseBuilder(client.cgi->response.data)
				.build()
		);
	}
}
