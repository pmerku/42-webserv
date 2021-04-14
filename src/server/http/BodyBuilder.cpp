//
// Created by mramadan on 22/03/2021.
//

#include "server/http/BodyBuilder.hpp"

using namespace NotApache;

const std::string BodyBuilder::_endLine = "\n";
const std::string BodyBuilder::_indent = "  ";
const std::string BodyBuilder::_tab = "\t";


BodyBuilder::BodyBuilder() {
	_body += "{";
	_body += _endLine;
}

BodyBuilder& BodyBuilder::fileName(const std::string& name) {
	_body += _indent + "\"file\": " + name;
	_body += _endLine;
	return *this;
}

BodyBuilder& BodyBuilder::fileSize(const std::string& size) {
	_body += _tab + "\"size\": " + size;
	_body += _endLine;
	return *this;
}

BodyBuilder& BodyBuilder::fileType(const std::string& type) {
	_body += _tab + "\"type\": " + type;
	_body += _endLine;
	return *this;
}

const std::string& BodyBuilder::build() {
	_body += "}";
	_body += _endLine;
	return _body;
}
