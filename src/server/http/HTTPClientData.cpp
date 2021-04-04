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
		currentPacket(data.data.begin())
{}

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
