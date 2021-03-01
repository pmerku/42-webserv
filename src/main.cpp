#include <iostream>
#include "Loggable.hpp"
#include "Logger.hpp"

class Something: public log::ILoggable {
public:
	Something() {}
};

int main() {
	log::Logger logger = std::cout;
	logger.setFlags(log::Flags::Debug | log::Flags::Color);

	Something hi;
	hi.setLogger(logger);

	hi.logItem("Hello world");
	hi.logItem(log::DEBUG, "Debug message here");
	hi.logItem(log::INFO, "Info message here");
	return 0;
}
