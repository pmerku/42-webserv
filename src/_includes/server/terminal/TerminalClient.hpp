//
// Created by jelle on 3/13/2021.
//

#ifndef TERMINALCLIENT_HPP
#define TERMINALCLIENT_HPP

#include "server/ServerTypes.hpp"
#include <string>

namespace NotApache {

	class TerminalClient {
	public:
		enum TerminalCommandState {
			FOUND_LINE,
			UNFINISHED
		};
	private:
		FD					_fd;
		std::string			_line;
		static const int	_bufferSize;
		bool 				_isClosed;

	public:
		TerminalClient(FD fd);

		bool 					isClosed() const;
		FD						getFd() const;
		TerminalCommandState	readNewData();
		TerminalCommandState	appendToLine(const std::string &str);
		TerminalCommandState	parseState() const;
		std::string				takeLine();

	};

}

#endif //TERMINALCLIENT_HPP
