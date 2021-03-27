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

HTTPClientResponse::HTTPClientResponse(): _response(), hasProgress(false), packetProgress(0), currentPacket(_response.begin()) {}

utils::DataList &HTTPClientResponse::getResponse() {
	return _response;
}

void HTTPClientResponse::setResponse(const utils::DataList &response) {
	_response = response;
}

utils::DataList &HTTPClientResponse::getAssociatedDataRaw() {
	return _associatedData;
}

void HTTPClientResponse::appendAssociatedData(const char *data, utils::DataList::size_type size) {
	_associatedData.add(data, size);
}
