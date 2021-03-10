//
// Created by jelle on 3/8/2021.
//

#include "config/AConfigBlock.hpp"
#include "config/ParseExceptions.hpp"

using namespace config;

void AConfigBlock::addLine(const ConfigLine &line) {
	runValidatorForKey(line);
	_lines.push_back(line);
}

void AConfigBlock::addBlock(AConfigBlock *block) {
	const std::string *arr = getAllowedBlocks();
	bool found = false;
	for (int i = 0; arr[i].length() != 0; ++arr) {
		if (block->getType() == arr[i]) {
			found = true;
			break;
		}
	}
	if (!found) {
		throw InvalidNestedBlockException(block->getType(), block->getLineNumber(), this);
	}
	_blocks.push_back(block);
}

AConfigBlock *AConfigBlock::getParent() const {
	return _parent;
}

void AConfigBlock::runPostValidators() {
	// run post validators
	for (std::vector<ConfigLine>::const_iterator i = _lines.begin(); i != _lines.end(); ++i) {
		validatorListType	validators = getValidatorForKey(*i);
		for (validatorListType::iterator j = validators.begin(); j < validators.end(); ++j) {
			if ((*j)->isPostValidator())
				(*j)->test(*i, *this);
		}
	}

	// run block validators
	ConfigLine testLine("a");
	for (validatorListType::const_iterator i = getBlockValidators().begin(); i != getBlockValidators().end(); ++i) {
		(*i)->test(testLine, *this);
	}

	// run post validators on children blocks;
	for (std::vector<AConfigBlock *>::const_iterator i = _blocks.begin(); i != _blocks.end(); ++i)
		(*i)->runPostValidators();
}

AConfigBlock::AConfigBlock(const ConfigLine &line, int lineNumber, AConfigBlock *parent): _parent(parent), _lineNumber(lineNumber), _isParsed(false) {
	if (line.getArgLength() != 1)
		throw ArgsWithBlockException(line);
	else if (line.getArg(0) != "{")
		throw BlockMissingOpeningException(line);
}

AConfigBlock::~AConfigBlock() {
	for (std::vector<AConfigBlock *>::iterator i = _blocks.begin(); i != _blocks.end(); ++i) {
		delete *i;
	}
}

void AConfigBlock::validateEndBlock(const ConfigLine &line) {
	if (line.getArgLength() != 0 || line.getKey() != "}" || line.getKey().length() > 1)
		throw ArgsWithBlockException(line);
}

const AConfigBlock::validatorListType &AConfigBlock::getValidatorForKey(const ConfigLine &line) const {
	validatorsMapType::const_iterator	it = getValidators().find(line.getKey());
	if (it == getValidators().end()) throw UnknownKeyException(line, 0);
	return it->second;
}

void AConfigBlock::runValidatorForKey(const ConfigLine &line) const {
	validatorListType	validators = getValidatorForKey(line);
	for (validatorListType::iterator i = validators.begin(); i < validators.end(); ++i) {
		if (!(*i)->isPostValidator())
			(*i)->test(line, *this);
	}
}

bool AConfigBlock::hasKey(const std::string &key) const {
	for (std::vector<ConfigLine>::const_iterator it = _lines.begin(); it != _lines.end(); ++it) {
		if (key == it->getKey()) return true;
	}
	return false;
}

const ConfigLine	*AConfigBlock::getKey(const std::string &key) const {
	for (std::vector<ConfigLine>::const_iterator it = _lines.begin(); it != _lines.end(); ++it) {
		if (key == it->getKey()) return &(*it);
	}
	return 0;
}

int AConfigBlock::getLineNumber() const {
	return _lineNumber;
}

void AConfigBlock::throwNotParsed() const {
	if (!_isParsed) throw NotParsedException();
}
