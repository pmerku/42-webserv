//
// Created by mramadan on 14/04/2021.
//

#include "plugins/JsonStatAPI.hpp"
#include "server/http/JsonBuilder.hpp"
#include "server/http/HTTPMimeTypes.hpp"
#include "server/http/HTTPMimeTypes.hpp"
#include "utils/ErrorThrow.hpp"

#include <sys/stat.h>

using namespace plugin;

JsonStatAPI::JsonStatAPI() : Plugin("json_stat_api") { }

JsonStatAPI::~JsonStatAPI() { }

bool JsonStatAPI::onFileServing(NotApache::HTTPClient& client) {
	struct stat buf;
	std::string body;

	if (::stat(client.file.path.c_str(), &buf) == -1)
		ERROR_THROW(NotFound());

	std::string fileName = client.file.path.substr(client.file.path.rfind("/")+1);
	std::string fileType = client.file.getExt();
	client.data.response.builder
		.setBody(NotApache::JsonBuilder()
			.addLine("name", fileName)
			.addLine("type", fileType)
			.addLine("size", buf.st_size)
			.addLine("MIMEType", MimeTypes::getMimeType(client.file.getExt()))
			.time("lastAccessed", buf.st_atime)
			.time("lastModified", buf.st_mtime)
			.time("lastStatusChange", buf.st_ctime)
			.mode("mode", buf.st_mode)
			.build()
		);
	client.data.response
		.setResponse(client.data.response.builder
			.setHeader("Content-Type", "application/json")
			.build()
		);
	return true;
}
