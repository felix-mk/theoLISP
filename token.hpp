#ifndef TOKEN_HPP
#define TOKEN_HPP

#include <unordered_map>

#include <string>
#include <string_view>

#include <ostream>

#include <cstdint>

#include "token_position.hpp"

enum class TokenType: uint8_t{
	CONTR_EOF,		// '\0'

	L_PAR,			// '('
	R_PAR,			// ')'

	IDENT,			// e.g. tumwiebrot
	INTEGER,		// e.g. 3141

	ADD,			// add
	SUB,			// sub
	MUL,			// mul

	SET,			// set

	IF,				// if
	WHILE			// while
};

static constexpr const char* const token_type_names[] = {
	"CONTR_EOF",

	"L_PAR",
	"R_PAR",

	"IDENT",
	"INTEGER",

	"ADD",
	"SUB",
	"MUL",

	"SET",

	"IF",
	"WHILE"	
};

static inline const char* token_type_name(const TokenType tt){
	return token_type_names[static_cast<uint8_t>(tt)];
}

static const std::unordered_map<std::string_view, TokenType> keyword_table{
	{"add", TokenType::ADD},
	{"sub", TokenType::SUB},
	{"mul", TokenType::MUL},
	{"set", TokenType::SET},
	{"if", TokenType::IF},
	{"while", TokenType::WHILE},
};

class Token{
	private:
		TokenType m_tt;
		std::string_view m_value;

		TokenPosition m_pos{};

	public:
		explicit Token(): m_tt{}, m_value{}, m_pos{}{
		}

		inline TokenType type()const{
			return this->m_tt;
		}

		inline const std::string_view& value()const{
			return this->m_value;
		}

		inline const char* name()const{
			return token_type_name(this->m_tt);
		}

		inline TokenPosition pos()const{
			return this->m_pos;
		}

		inline void set(const TokenType tt, const TokenPosition pos){
			this->set(tt, std::string_view{}, pos);
		}

		inline void set(const TokenType tt, const std::string_view value, const TokenPosition pos){
			this->m_tt = tt;
			this->m_value = value;
			this->m_pos = pos;
		}

		friend inline bool operator== (const Token& token, const TokenType tt){
			return token.type() == tt;
		}

		friend inline bool operator!= (const Token& token, const TokenType tt){
			return !(token == tt);
		}

		friend std::ostream& operator<< (std::ostream& os, const Token& token){
			return os << token.name() << " [" << token.value() << "] (" << token.pos() << ')';
		}
};

#endif	// TOKEN_HPP