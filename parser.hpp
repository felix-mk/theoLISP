#ifndef PARSER_HPP
#define PARSER_HPP

#include <string>

#include <sstream>
#include <iostream>

#include <cstdint>

#include "token.hpp"
#include "lexer.hpp"
#include "ast_node.hpp"
#include "util.hpp"

class Parser{
	private:
		Token m_token;
		Lexer m_lexer;

	public:
		explicit Parser(const std::string& code)noexcept:
			m_token{}, m_lexer{code}{
		}

		BaseNode* parse()noexcept{
			this->read_next_token();
			BaseNode* const root = this->parse_start();
			this->expect(TokenType::CONTR_EOF);

			return root;
		}

	private:
		// start ::= instr_list;
		inline BaseNode* parse_start()noexcept{
			return this->parse_instr_list();
		}

		// instr_list ::= '(' {instr} ')';
		BaseNode* parse_instr_list()noexcept{
			const TokenPosition pos = this->m_token.pos();

			this->expect(TokenType::L_PAR);
			this->read_next_token();

			InstrListNode* list = new InstrListNode{pos};
			while(this->m_token != TokenType::R_PAR && this->m_token != TokenType::CONTR_EOF)
				list->add(this->parse_instr());

			this->expect(TokenType::R_PAR);
			this->read_next_token();

			return list;
		}

		// instr ::= '(' (assign | cond | loop) ')';
		BaseNode* parse_instr()noexcept{
			BaseNode* ret{};

			this->expect(TokenType::L_PAR);
			this->read_next_token();

			switch(this->m_token.type()){
				case TokenType::SET:
					ret = this->parse_assign();
					break;
				case TokenType::IF:
					ret = this->parse_cond();
					break;
				case TokenType::WHILE:
					ret = this->parse_loop();
					break;
				default:
					this->expect(
						TokenType::SET,
						TokenType::IF,
						TokenType::WHILE
					);
					this->read_next_token();

					ret = new ErrorNode{this->m_token.pos()};
			}

			this->expect(TokenType::R_PAR);
			this->read_next_token();

			return ret;
		}

		// assign ::= 'set' ident exp;
		BaseNode* parse_assign()noexcept{
			const TokenPosition pos = this->m_token.pos();

			this->debug_expect(TokenType::SET);
			this->read_next_token();

			if(this->m_token == TokenType::IDENT){
				const std::string_view var_name = this->m_token.value();
				this->read_next_token();
				BaseNode* const value = this->parse_exp();

				return new AssignNode{var_name, value, pos};
			}else{
				this->expect(TokenType::IDENT);
				this->read_next_token();

				return new ErrorNode{pos};
			}
		}

		// cond ::= 'if' exp instr_list instrs_list;
		BaseNode* parse_cond()noexcept{
			const TokenPosition pos = this->m_token.pos();

			this->debug_expect(TokenType::IF);
			this->read_next_token();

			BaseNode* const condition = this->parse_exp();
			BaseNode* const if_branch = this->parse_instr_list();
			BaseNode* const else_branch = this->parse_instr_list();

			return new IfNode{condition, if_branch, else_branch, pos};
		}

		// loop ::= 'while' exp instr_list;
		BaseNode* parse_loop()noexcept{
			const TokenPosition pos = this->m_token.pos();

			this->debug_expect(TokenType::WHILE);
			this->read_next_token();

			BaseNode* const condition = this->parse_exp();
			BaseNode* const loop = this->parse_instr_list();

			return new WhileNode{condition, loop, pos};
		}

		// exp ::= integer | ident | '(' arith_exp ')';
		// ident	::= ('a' | ... | 'z') {'a' | ... | 'z' | '0' | ... | '9'};
		// integer	::= '0' | (('1' | ... | '9') {'0' | ... | '9'});
		BaseNode* parse_exp()noexcept{
			BaseNode* ret{};
			const TokenPosition pos = this->m_token.pos();

			switch(this->m_token.type()){
				case TokenType::INTEGER:
					ret = new IntNode{
						sv_to_int(this->m_token.value()),
						pos
					};
					this->read_next_token();
					break;
				case TokenType::IDENT:
					ret = new VarNode{this->m_token.value(), pos};
					this->read_next_token();
					break;
				case TokenType::L_PAR:
					this->read_next_token();
					ret = this->parse_arith_exp();
					this->expect(TokenType::R_PAR);
					this->read_next_token();
					break;
				default:
					this->expect(
						TokenType::INTEGER,
						TokenType::IDENT,
						TokenType::L_PAR
					);
					this->read_next_token();
			}

			return ret;
		}

		// arith_exp ::= ('add' | 'sub' | 'mul') exp exp;
		BaseNode* parse_arith_exp()noexcept{
			const TokenPosition pos = this->m_token.pos();
			const TokenType arith_type = this->m_token.type();

			this->read_next_token();
			BaseNode* const param1 = this->parse_exp();
			BaseNode* const param2 = this->parse_exp();

			switch(arith_type){
				case TokenType::ADD:
					return new AddNode{param1, param2, pos};
				case TokenType::SUB:
					return new SubNode{param1, param2, pos};
				case TokenType::MUL:
					return new MulNode{param1, param2, pos};
				default:
					this->expect(
						TokenType::ADD,
						TokenType::SUB,
						TokenType::MUL
					);
					this->read_next_token();

					delete param1; delete param2;
					return new ErrorNode{pos};
			}
		}

		template <typename... T>
		void expect(const T... tt)const noexcept{
			static_assert(sizeof...(T) > 0);

			if(((this->m_token != tt) && ...)){
				std::ostringstream oss{};
				oss << "error[parser, " << this->m_token.pos() << "]: "
					<< "Invalid token " << this->m_token.name()
					<< " (";

				print_list(oss, token_type_name(tt)...);
				oss << " expected).\n";

				std::cerr << oss.str();
			}
		}

		template <typename... T>
		void debug_expect([[maybe_unused]] const T... tt)const noexcept{
#			ifdef DEBUG
				this->expect(tt...);
#			endif
		}

		inline void read_next_token()noexcept{
			this->m_lexer.read_next_token(this->m_token);
		}
};

#endif	// PARSER_HPP