//
// Created by pmerku on 11/03/2021.
//

#ifndef ARESPONSE_HPP
#define ARESPONSE_HPP

#include <map>
#include <string>
#include "server/http/HTTPClient.hpp"

namespace response {

	class AResponse {
	private:
		std::string _response;

	public:
		AResponse();
		virtual ~AResponse();

		const std::string &getResponse() const;
		void setResponse(const NotApache::HTTPClient &client);
	};

} // namespace response

#endif //ARESPONSE_HPP
