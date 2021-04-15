//
// Created by jelle on 4/12/2021.
//

#include "server/http/HTTPResponder.hpp"
#include "libs/duktape/duktape.h"
#include <unistd.h>
#include <cstdlib>
#include <cstring>

using namespace NotApache;

static duk_ret_t printCBinding(duk_context *ctx) {
	const char *str = duk_to_string(ctx, 0);
	ssize_t ret = write(STDOUT_FILENO, str, strlen(str));
	(void)ret;
	return 0;
}

void	HTTPResponder::runJs(HTTPClient& client) {
	client.cgi = new CgiClass; // we are reusing the CGI class, it has everything we need for JS

	if (::pipe(client.cgi->pipefd) == -1) {
		ERROR_THROW(CgiClass::PipeFail());
	}

	duk_context *ctx = duk_create_heap_default();
	if (!ctx)
		ERROR_THROW(CgiClass::CGIException());
	for (utils::DataList::iterator it = client.data.response.getAssociatedDataRaw().begin(); it != client.data.response.getAssociatedDataRaw().end(); ++it) {
		if (it->size > 0) {
			duk_push_lstring(ctx, it->data, it->size);
		}
	}
	client.data.response.getAssociatedDataRaw().clear(); // free unused data so it doesnt eat all the ram

	client.cgi->pid = ::fork();
	client.cgi->hasExited = false;
	if (client.cgi->pid == -1) {
		client.cgi->closePipes(&client.cgi->pipefd[0], &client.cgi->pipefd[1], 0, 0);
		ERROR_THROW(CgiClass::ForkFail());
	}
	if (!client.cgi->pid) {
		// CHILD PROCESS

		// set output pipes
		if (::dup2(client.cgi->pipefd[1], STDOUT_FILENO) == -1)
			::exit(DUP2_ERROR);
		if (::close(client.cgi->pipefd[0]) == -1 || ::close(client.cgi->pipefd[1]) == -1)
			::exit(CLOSE_ERROR);

		// run JavaScript
		duk_push_c_function(ctx, printCBinding, 1);
		duk_put_global_string(ctx, "print");
		duk_eval_noresult(ctx);
		duk_destroy_heap(ctx);

		// exit success
		::exit(0);
	}

	// CURRENT PROCESS
	duk_destroy_heap(ctx);
	try {
		client.cgi->closePipes(0, &client.cgi->pipefd[1], 0, 0);
	} catch (std::exception &e) {
		client.cgi->closePipes(&client.cgi->pipefd[0], 0, 0, 0);
		throw;
	}

	client.removeAssociatedFd(client.getAssociatedFd(0).fd); // remove previous file FD
	client.addAssociatedFd(client.cgi->pipefd[0]);
	client.responseState = CGI;
	client.connectionState = ASSOCIATED_FD;
}
