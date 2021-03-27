//
// Created by jelle on 3/14/2021.
//

#include "server/http/HTTPClientData.hpp"

using namespace NotApache;

HTTPClientRequest::HTTPClientRequest(): _rawRequest(), _method(), _uri(), _headers(), _body(), _statusCode(), _isChunked(false) {}

const std::string &HTTPClientRequest::getRawRequest() const {
	return _rawRequest;
}

void HTTPClientRequest::appendRequestData(const std::string &newData) {
	_rawRequest += newData;
}

void HTTPClientRequest::setRawRequest(const std::string &newData) {
	_rawRequest = newData;
}

HTTPClientResponse::HTTPClientResponse(): _response(),  _progress(0) {}

const std::string &HTTPClientResponse::getResponse() const {
	return _response;
}

std::string::size_type HTTPClientResponse::getProgress() const {
	return _progress;
}

void HTTPClientResponse::setResponse(const std::string &response) {
	_response = response;
}

void HTTPClientResponse::setProgress(std::string::size_type index) {
	_progress = index;
}

const std::string &HTTPClientResponse::getAssociatedDataRaw() const {
	return _associatedData;
}

void HTTPClientResponse::appendAssociatedData(const std::string &newData) {
	_associatedData += newData;
}

void HTTPClientResponse::setAssociatedData(const std::string &newData) {
	_associatedData = newData;
}
