//
// Created by mramadan on 14/04/2021.
//

#include "plugins/JsonStatAPI.hpp"
#include "server/http/BodyBuilder.hpp"
#include "server/http/HTTPMimeTypes.hpp"
#include "server/http/HTTPMimeTypes.hpp"
#include "utils/ErrorThrow.hpp"

#include <sys/stat.h>

using namespace plugin;

JsonStatAPI::JsonStatAPI() : Plugin("json_stat_api") { }

JsonStatAPI::~JsonStatAPI() { }

bool JsonStatAPI::onBeforeFileServing(NotApache::HTTPClient& client) {
	struct stat buf;
	std::string body;

	if (::stat(client.file.path.c_str(), &buf) == -1)
		ERROR_THROW(NotFound());

	std::string fileName = client.file.path.substr(client.file.path.rfind("/")+1);
	std::string fileType = client.file.path.substr(client.file.path.rfind(".")+1);
	client.data.response.builder.setBody(NotApache::BodyBuilder()
		.line("name", fileName)
		.line("type", fileType)
		.line("size", buf.st_size)
		.line("MIME type", MimeTypes::getMimeType(client.file.getExt()))
		.time("last accessed", buf.st_atime)
		.time("last modified", buf.st_mtime)
		.time("last status change", buf.st_ctime)
		.mode("mode", buf.st_mode)
		.build()
	);

	return true;
}
