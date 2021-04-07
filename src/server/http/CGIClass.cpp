//
// Created by pmerku on 30/03/2021.
//

#include "server/http/CGIClass.hpp"
#include "utils/ErrorThrow.hpp"

using namespace NotApache;

CgiClass::CgiClass() : response(HTTPParseData::CGI_RESPONSE) {}

CgiClass::~CgiClass() {}
