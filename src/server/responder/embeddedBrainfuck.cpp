//
// Created by jelle on 4/12/2021.
//

#include "server/http/HTTPResponder.hpp"
#include <unistd.h>
#include <cstdlib>
#include <cstring>

using namespace NotApache;

#define BRAINFUCKMEMORYSIZE 30000
#define BRAINFUCKMAXINSTRUCTIONS 300000
struct brainfuckContext {
	unsigned char *ptr;
	unsigned char *minPtr;
	unsigned char *maxPtr;
	unsigned long instructionCount;
};

static const std::string brainfuckChars = "[]<>+-.,";

static bool	runBrainfuckCharacter(unsigned char c, brainfuckContext &ctx, utils::DataList::DataListIterator &it, utils::DataList &list) {
	if (brainfuckChars.find(c) == std::string::npos)
		return true;
	if (c == '+')
		++(*ctx.ptr);
	else if (c == '-')
		--(*ctx.ptr);
	else if (c == '>') {
		// out of bounds check
		if (ctx.ptr == ctx.maxPtr)
			return false;
		++ctx.ptr;
	}
	else if (c == '<') {
		// out of bounds check
		if (ctx.ptr == ctx.minPtr)
			return false;
		--ctx.ptr;
	}
	else if (c == '.') {
		if (write(STDOUT_FILENO, ctx.ptr, 1) == -1)
			return false;
	}
	else if (c == ',')
		return false; // no support for input data, just exit program
	else if (c == '[') {
		// jump instruction pointer to matching end loop block
		if (*ctx.ptr == 0) {
			int state = 1;
			do {
				++it;
				if (it == list.endList())
					return false;
				if (*it == '[')
					state++;
				else if (*it == ']')
					state--;
			} while (state != 0);
		}
	}
	else if (c == ']') {
		// jump instruction pointer to matching begin loop block
		if (*ctx.ptr != 0) {
			int state = 1;
			do {
				if (it == list.beginList())
					return false;
				--it;
				if (*it == '[')
					state--;
				else if (*it == ']')
					state++;
			} while (state != 0);
		}
	}
	return true;
}

void	HTTPResponder::runBrainfuck(HTTPClient& client) {
	client.cgi = new CgiClass; // we are reusing the CGI class, it has everything we need

	if (::pipe(client.cgi->pipefd) == -1) {
		ERROR_THROW(CgiClass::PipeFail());
	}

	client.cgi->pid = ::fork();
	client.cgi->hasExited = false;
	if (client.cgi->pid == -1) {
		client.cgi->closePipes(&client.cgi->pipefd[0], &client.cgi->pipefd[1], 0, 0);
		ERROR_THROW(CgiClass::ForkFail());
	}
	if (!client.cgi->pid) {
		// CHILD PROCESS
		utils::DataList &file = client.data.response.getAssociatedDataRaw();
		unsigned char *buf = new unsigned char[BRAINFUCKMEMORYSIZE]();
		brainfuckContext ctx = {};
		ctx.ptr = buf;
		ctx.minPtr = ctx.ptr;
		ctx.maxPtr = ctx.minPtr + BRAINFUCKMEMORYSIZE;
		ctx.instructionCount = 0;

		// set output pipes
		if (::dup2(client.cgi->pipefd[1], STDOUT_FILENO) == -1)
			::exit(DUP2_ERROR);
		if (::close(client.cgi->pipefd[0]) == -1 || ::close(client.cgi->pipefd[1]) == -1)
			::exit(CLOSE_ERROR);

		// send cgi header, error if not fully send
		if (::write(STDOUT_FILENO, "Content-Type: text/plain\r\n\r\n", 28) != 28)
			::exit(EXECVE_ERROR);

		// run brainfuck
		for (utils::DataList::DataListIterator it = file.beginList(); it != file.endList(); ++it) {
			if (!runBrainfuckCharacter((unsigned char)(*it), ctx, it, file))
				::exit(EXECVE_ERROR);
			ctx.instructionCount++;
			if (ctx.instructionCount >= BRAINFUCKMAXINSTRUCTIONS)
				::exit(EXECVE_ERROR);
		}

		// exit success
		delete [] buf;
		::exit(0);
	}

	// CURRENT PROCESS
	// free unused data
	client.data.response.getAssociatedDataRaw().clear();
	try {
		client.cgi->closePipes(0, &client.cgi->pipefd[1], 0, 0);
	} catch (std::exception &e) {
		client.cgi->closePipes(&client.cgi->pipefd[0], 0, 0, 0);
		throw;
	}

	client.addAssociatedFd(client.cgi->pipefd[0]);
	client.responseState = CGI;
	client.connectionState = ASSOCIATED_FD;
}
