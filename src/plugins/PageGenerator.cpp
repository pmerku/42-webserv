//
// Created by pmerku on 14/04/2021.
//

#include <cstdlib>
#include "plugins/PageGenerator.hpp"
#include "utils/intToString.hpp"
#include "utils/CreateVector.hpp"

using namespace plugin;

const std::vector<std::string> PageGenerator::_facts =
	utils::CreateVector<std::string>
		("Odontophobia is the fear of teeth.")
		("The Neanderthal's brain was bigger than yours is.")
		("The international telephone dialing code for Antarctica is 672.")
		("Did you hear about the Italian chef that died? He pasta-away.")
		("I just got my doctor's test results and I'm really upset. Turns out, I'm not gonna be a doctor.")
		("The inventor of the Pringles can is now buried in one.")
		("According to British law, any unclaimed swan swimming in the open waters of England and Wales belongs to the Queen.")
		("You are 13.8 percent more likely to die on your birthday.")
		("Mulan has the highest kill-count of any Disney character.")
		("You can eat and drink anything at least once.")
		("What's red and bad for your teeth? A brick.")
		("An apple a day keeps the doctor away. Or at least it does if you throw it hard enough.")
		("My son, who's into astronomy, asked me how stars die. \"Usually an overdose, son,\" I told him.")
		("If you donate one kidney, everybody loves you, and you're a total hero. But donate five and suddenly everyone is yelling. Sheesh!")
		("Banging your head against a wall uses 150 calories an hour.")
		("Chuck Norris counted to infinity, twice.")
		("A group of frogs is called an army.")
		("Walmart has a lower acceptance rate than Harvard.")
		("China censored the word for \"censorship.\"")
		("You're twice as likely to be killed by a vending machine than a shark.")
		("\"I'm sorry\" and \"I apologize\" mean the same thing. Except at a funeral.")
		;

const std::string PageGenerator::_template =
		std::string("<!DOCTYPE html>"
  		"<html>"
		"<head><title>404 Not Found</title><style>"
		"*{transition:all 0.6s;}"
		"html{"
  		"height:100%;"
		"background-image:url(\"https://picsum.photos/1920/1080\");"
  		"background-color:#E0E0E0;"
		"background-repeat:no-repeat;"
  		"background-size:cover;}"
		"body{"
  		"font-family:'Lato', sans-serif;"
		"color:#575656;"
  		"margin:0;}"
		"#main{"
  		"display:table;"
		"width:100%;"
  		"height:100vh;"
		"text-align:center;}"
		".fof{"
  		"display:table-cell;"
		"vertical-align:middle;}"
		".box{"
  		"background-color:#E0E0E0;"
		"padding-bottom:20px;"
  		"margin-left:350px;"
		"margin-right:350px;}"
		".fof h1{"
  		"font-size:80px;"
		"display:inline-block;"
  		"padding-right:12px;"
		"animation: type .5s alternate infinite;}"
		".fof p{font-size:20px;}"
		"@keyframes type{"
  		"from{box-shadow: inset -3px 0px 0px #575656;}"
		"to{box-shadow:inset -3px 0px 0px transparent;}}"
		"</style></head><body>"
		"<div id=\"main\"><div class=\"fof\"><div class=\"box\">"
		"<br><h1>Error 404</h1><br>"
		"<p>{FACT}</p>"
		"</div></div></div></body></html>");

PageGenerator::PageGenerator() : Plugin("page_404_gen") {
	std::srand(time(0)); // set seed only once
}

PageGenerator::~PageGenerator() { }

bool PageGenerator::onHandleError(NotApache::HTTPClient &client, int code) {
	if (code == 404) {
		NotApache::globalLogger.logItem(logger::DEBUG, "Page Generator running");
		client.data.response.builder
			// set response code
			.setStatus(code)
			// set mime type
			.setHeader("Content-Type", "text/html");

		// find {FACT} in template and replace with a string from the _facts vector
		std::string::size_type i = _template.find("{FACT}");
		std::string out = _template;
		if (i != std::string::npos) {
			out.replace(i, 6, _facts[std::rand() % _facts.size()]);
		}
		// generate html code
		client.data.response.builder.setBody(out);
		client.data.response.setResponse(client.data.response.builder.build());
		return true;
	}
	return false;
}
