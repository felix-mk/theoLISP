#ifndef LEXER_HPP
#define LEXER_HPP

#include <string>
#include <string_view>

#include <sstream>
#include <iostream>

#include <cctype>
#include <cstdint>

#include "token.hpp"
#include "util.hpp"

class Lexer{
	private:
		char m_chr;
		std::string::const_iterator m_it;
		std::string::const_iterator m_end;

		TokenPosition m_pos{};

	public:
		Lexer(const std::string& code)noexcept:
			m_chr{'\0'}, m_it{code.cbegin()}, m_end{code.cend()}{

			if(this->m_it != this->m_end)
				this->m_chr = *this->m_it;
		}

		void read_next_token(Token& token)noexcept{
			while(true){
				switch(this->m_chr){
					case '\n':
						this->m_pos.inc_line();
					[[fallthrough]];
					case ' ': case '\t':
						this->read_next_char();
						break;
					case '\0':
						token.set(TokenType::CONTR_EOF, this->m_pos);
						return;
					case '(':
						token.set(TokenType::L_PAR, this->m_pos);
						this->read_next_char();
						return;
					case ')':
						token.set(TokenType::R_PAR, this->m_pos);
						this->read_next_char();
						return;
					case '0':
						token.set(TokenType::INTEGER, ZERO_SV, this->m_pos);
						this->read_next_char();
						return;
					case '1': case '2': case '3':
					case '4': case '5': case '6':
					case '7': case '8': case '9': {
						TokenPosition int_pos{this->m_pos.col(), this->m_pos.line()};
						std::string::const_iterator int_begin = this->m_it;

						do{
							this->read_next_char();
						}while(std::isdigit(this->m_chr));

						std::string_view buffer = sv_from_range(int_begin, this->m_it);
						token.set(TokenType::INTEGER, buffer, int_pos);
						return;
					}
					case 'a': case 'b': case 'c': case 'd':
					case 'e': case 'f': case 'g': case 'h':
					case 'i': case 'j': case 'k': case 'l':
					case 'm': case 'n': case 'o': case 'p':
					case 'q': case 'r': case 's': case 't':
					case 'u': case 'v': case 'w': case 'x':
					case 'y': case 'z': {
						TokenPosition ident_pos{this->m_pos.col(), this->m_pos.line()};
						std::string::const_iterator ident_begin = this->m_it;

						do{
							this->read_next_char();
						}while(std::isalnum(this->m_chr));

						std::string_view buffer = sv_from_range(ident_begin, this->m_it);

						const auto res = keyword_table.find(buffer);
						if(res != keyword_table.end())
							token.set(res->second, ident_pos);
						else
							token.set(TokenType::IDENT, buffer, ident_pos);

						return;
					}
					default: {
						std::ostringstream oss{};
						oss << "error[lexer, " << this->m_pos << "]: invalid char \'" << this->m_chr << '\''
							<< " (ASCII: " << static_cast<uint16_t>(this->m_chr) << ").\n";

						std::cerr << oss.str();
						this->read_next_char();
					}
				}
			}

			token.set(TokenType::CONTR_EOF, this->m_pos);
		}

	private:
		inline void read_next_char()noexcept{
			this->m_pos.inc_col();
			this->m_chr = (this->m_it != this->m_end) ? *++this->m_it : '\0';
		}
};

#endif	// LEXER_HPP