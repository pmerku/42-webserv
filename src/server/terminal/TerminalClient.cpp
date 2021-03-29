//
// Created by jelle on 3/13/2021.
//

#include "server/terminal/TerminalClient.hpp"
#include "server/global/GlobalLogger.hpp"
#include <unistd.h>
#include <fcntl.h>

using namespace NotApache;

const int	TerminalClient::_bufferSize = 1024;

TerminalClient::TerminalClient(FD fd): _fd(fd), _line(), _isClosed(false) {
	fcntl(_fd, F_SETFL, O_NONBLOCK);
}

TerminalClient::TerminalCommandState TerminalClient::readNewData() {
	char		buf[_bufferSize+1];
	::size_t	len = _bufferSize;
	::ssize_t	ret = ::read(_fd, buf, len);
	if (ret == -1) {
		globalLogger.logItem(logger::DEBUG, "Failed to read from terminal, assuming FD as closed");
		_isClosed = true;
		return parseState();
	}
	buf[ret] = 0; // makes cstr out of buffer
	return appendToLine(buf);
}

TerminalClient::TerminalCommandState TerminalClient::appendToLine(const std::string &str) {
	_line += str;
	return parseState();
}

TerminalClient::TerminalCommandState TerminalClient::parseState() const {
	std::string::size_type	newlinePos = _line.find('\n');
	if (newlinePos == std::string::npos)
		return UNFINISHED;
	return FOUND_LINE;
}

std::string TerminalClient::takeLine() {
	std::string::size_type	newlinePos = _line.find('\n');
	std::string	newStr = _line.substr(0, newlinePos);
	_line = _line.substr(newlinePos+1);
	return newStr;
}

bool TerminalClient::isClosed() const {
	return _isClosed;
}

FD TerminalClient::getFd() const {
	return _fd;
}
