//
// Created by pmerku on 3/1/2021.
//

#ifndef REGEX_HPP
#define REGEX_HPP

#include <string>

namespace regex {

#define MAX_REGEXP_SYMBOLS 64
#define MAX_REGEXP_BUFFER 128

#define MAX_QUANTITY 1024
#define MAX_PLUS_STAR 4000

#define MATCH_DIGIT(c) ((c >= '0') && (c <= '9'))
#define MATCH_ALPHA(c) ((c >= 'A') && (c <= 'z')) || ((c >= 'A') && (c <= 'Z'))
#define MATCH_WHITESPACE(c) ((c == ' ') || (c == '\t') || (c == '\n') || (c == '\r') || (c == '\f') || (c == '\v'))
#define MATCH_ALPHANUM(c) ((c == '_') || MATCH_ALPHA(c) || MATCH_DIGIT(c))
#define MATCH_DOT(c) ((c != '\n') && (c != '\r'))
#define IS_METACHAR(c) ((c == 's') || (c == 'S') || (c == 'w') || (c == 'W') || (c == 'd') || (c == 'D'))

	enum {
		NONE,
		DOT,
		BEGIN,
		END,
		QUANTIFIER,
		QUANTIFIER_LAZY,
		QUESTIONMARK,
		QUESTIONMARK_LAZY,
		STAR,
		STAR_LAZY,
		PLUS,
		PLUS_LAZY,
		CHAR,
		CHAR_CLASS,
		INV_CHAR_CLASS,
		DIGIT,
		NOT_DIGIT,
		ALPHA,
		NOT_ALPHA,
		WHITESPACE,
		NOT_WHITESPACE
	};

	struct RegexNode {
		unsigned char type;
		union {
			unsigned char ch;
			unsigned char *cc;
			unsigned short mn[2];
		} u;
	};

	struct Regexp {
		struct RegexNode regexNodes[MAX_REGEXP_SYMBOLS];
		unsigned char regexBuffer[MAX_REGEXP_BUFFER];
	};

	// WARNING, not copy safe, do not attempt to assign without using pointers
	class Regex {
	private:
		struct Regexp _regex;

		const char *matchPattern(RegexNode *nodes, const char *text, const char *textEnd);
		const char *matchQuantity(RegexNode *nodes, const char *text, const char *textEnd, unsigned min, unsigned max);
		const char *matchQuantityLazy(RegexNode *nodes, const char *text, const char *textEnd, unsigned min, unsigned max);

		int matchOne(RegexNode *node, unsigned char c);
		int matchCharClass(unsigned char c, const char *str);
		static int matchMetaChar(unsigned char c, unsigned char mc);

	public:
		explicit Regex(const std::string &pattern);

		/**
		 * Compiles the regex pattern into nodes
		 * @param pattern regex pattern
		 */
		void compile(const std::string &pattern);

		/**
		 * Finds the compiled pattern match in the text. It will return true
		 * only if the text matches fully
		 * @param text text to match with compiled pattern
		 * @return bool
		 */
		bool match(const std::string &text);

		class RegexCompilationError : public std::exception {
		public:
			virtual const char *what() const throw() {
				return "Failed to compile regex";
			}
		};

		class ExceededBufferLimit : public RegexCompilationError {
		public:
			const char *what() const throw() {
				return "Exceeded buffer size limit";
			}
		};

		class OutOfBoundAccess : public RegexCompilationError {
		public:
			const char *what() const throw() {
				return "Pattern went out of bounds";
			}
		};

		class ImpossiblePattern : public RegexCompilationError {
		public:
			const char *what() const throw() {
				return "There is something wrong with the pattern";
			}
		};

		class ImpossibleValue : public RegexCompilationError {
		public:
			const char *what() const throw() {
				return "The min/max quantifier value is not possible";
			}
		};

		class NoTextToMatch : public RegexCompilationError {
		public:
			const char *what() const throw() {
				return "The provided text is empty";
			}
		};
	};

} // namespace regex

#endif //REGEX_HPP
