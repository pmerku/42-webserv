//
// Created by jelle on 3/14/2021.
//

#include "server/http/HTTPClientData.hpp"

using namespace NotApache;

HTTPClientRequest::HTTPClientRequest():
	_associatedData(),
	data(),
	hasProgress(false),
	packetProgress(0),
	currentPacket(data.data.begin()) {}

HTTPClientResponse::HTTPClientResponse(HTTPParseData::HTTPParseType type) :
	_associatedData(),
	data(type),
	hasProgress(false),
	packetProgress(0),
	currentPacket(data.data.begin()) {}

utils::DataList &HTTPClientRequest::getRequest() {
	return data.data;
}

void HTTPClientRequest::setRequest(const utils::DataList &request) {
	data.data = request;
}

utils::DataList &HTTPClientRequest::getAssociatedDataRaw() {
	return _associatedData;
}

void HTTPClientRequest::appendAssociatedData(const char *d, utils::DataList::size_type size) {
	_associatedData.add(d, size);
}

void HTTPClientRequest::appendRequestData(const char *d, utils::DataList::size_type size) {
	data.data.add(d, size);
}

HTTPClientResponse::HTTPClientResponse():
		_associatedData(),
		data(),
		hasProgress(false),
		packetProgress(0),
		currentPacket(data.data.begin()) {}

utils::DataList &HTTPClientResponse::getResponse() {
	return data.data;
}

void HTTPClientResponse::setResponse(const utils::DataList &response) {
	data.data = response;
}

utils::DataList &HTTPClientResponse::getAssociatedDataRaw() {
	return _associatedData;
}

void HTTPClientResponse::appendAssociatedData(const char *d, utils::DataList::size_type size) {
	_associatedData.add(d, size);
}

void HTTPClientResponse::appendResponseData(const char *d, utils::DataList::size_type size) {
	data.data.add(d, size);
}

void HTTPClientData::reset() {
	request = HTTPClientRequest();
	response = HTTPClientResponse();
}
