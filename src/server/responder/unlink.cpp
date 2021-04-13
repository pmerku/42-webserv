//
// Created by jelle on 4/12/2021.
//

#include "server/http/HTTPResponder.hpp"
#include <cerrno>
#include <unistd.h>

using namespace NotApache;

void HTTPResponder::deleteFile(HTTPClient &client) {
	struct ::stat buf = {};

	// get file data
	if (::stat(client.file.path.c_str(), &buf) == -1) {
		if (errno == ENOENT || errno == ENOTDIR)
			handleError(client, 404);
		else
			handleError(client, 500);
		return;
	}

	// only allow removing of normal files
	if (!S_ISREG(buf.st_mode)) {
		handleError(client, 403);
		return;
	}

	// remove the file
	int result = ::unlink(client.file.path.c_str());
	if (result == -1) {
		handleError(client, 500);
		return;
	}

	client.replyStatus = 200;
	client.data.response.setResponse(
		ResponseBuilder()
			.setHeader("Content-Type", "text/html")
			.setBody("<h1>Successfully deleted file!</h1>")
			.build()
	);
}
