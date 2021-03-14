//
// Created by jelle on 3/14/2021.
//

#include "server/global/GlobalLogger.hpp"
#include "server/global/GlobalConfig.hpp"

namespace NotApache {
	config::RootBlock	*configuration = 0;
	logger::ILoggable	globalLogger = logger::ILoggable();
}
