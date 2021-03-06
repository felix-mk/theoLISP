#ifndef PARSER_HPP
#define PARSER_HPP

#include <string>

#include <sstream>
#include <iostream>

#include <cstdint>

#include "token.hpp"
#include "lexer.hpp"
#include "ast_node.hpp"

#include "args.hpp"
#include "util.hpp"

class Parser{
	private:
		Token m_token;
		Lexer m_lexer;

		mutable bool m_ok;

	public:
		explicit Parser(const std::string& code):
			m_token{}, m_lexer{code}, m_ok{true}{
		}

		BaseNode* parse(){
			this->read_next_token();
			BaseNode* const root = this->parse_start();
			this->expect(TokenType::CONTR_EOF);

			return root;
		}

	private:
		// start ::= instr_list;
		inline BaseNode* parse_start(){
			return this->parse_instr_list();
		}

		// instr_list ::= '(' {instr} ')';
		BaseNode* parse_instr_list(){
			const TokenPosition pos = this->m_token.pos();

			this->expect_and_read(TokenType::L_PAR);

			InstrListNode* list = new InstrListNode{pos};
			while(this->m_token != TokenType::R_PAR && this->m_token != TokenType::CONTR_EOF && this->m_ok){
				const TokenPosition prev_pos = this->m_token.pos();
				list->add(this->parse_instr());

				// Prevention of endless loops in case of syntax errors.
				if(prev_pos == this->m_token.pos())
					this->read_next_token();
			}

			this->expect_and_read(TokenType::R_PAR);
			return list;
		}

		// instr ::= '(' (assign | cond | loop) ')';
		BaseNode* parse_instr(){
			BaseNode* ret{};

			this->expect_and_read(TokenType::L_PAR);
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

					ret = new ErrorNode{this->m_token.pos()};
			}

			this->expect_and_read(TokenType::R_PAR);

			return ret;
		}

		// assign ::= 'set' ident exp;
		BaseNode* parse_assign(){
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
				return new ErrorNode{pos};
			}
		}

		// cond ::= 'if' exp instr_list instrs_list;
		BaseNode* parse_cond(){
			const TokenPosition pos = this->m_token.pos();

			this->debug_expect(TokenType::IF);
			this->read_next_token();

			BaseNode* const condition = this->parse_exp();
			BaseNode* const if_branch = this->parse_instr_list();
			BaseNode* const else_branch = this->parse_instr_list();

			return new IfNode{condition, if_branch, else_branch, pos};
		}

		// loop ::= 'while' exp instr_list;
		BaseNode* parse_loop(){
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
		BaseNode* parse_exp(){
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
					this->expect_and_read(TokenType::R_PAR);
					break;
				default:
					this->expect(
						TokenType::INTEGER,
						TokenType::IDENT,
						TokenType::L_PAR
					);
			}

			return ret;
		}

		// arith_exp ::= ('add' | 'sub' | 'mul') exp exp;
		BaseNode* parse_arith_exp(){
			const TokenPosition pos = this->m_token.pos();
			const TokenType arith_type = this->m_token.type();

			this->expect_and_read(TokenType::ADD, TokenType::SUB, TokenType::MUL);

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
					delete param1; delete param2;
					return new ErrorNode{pos};
			}
		}

		template <typename... T>
		bool expect(const T... tt)const{
			static_assert(sizeof...(T) > 0);

			if(((this->m_token != tt) && ...)){
				std::ostringstream oss{};
				oss << "error[parser, " << this->m_token.pos() << "]: "
					<< "Invalid token " << this->m_token.name()
					<< " (";

				print_list(oss, token_type_name(tt)...);
				oss << " expected).\n";

				std::cerr << oss.str();

				if(args.try_recovery_from_syntax_errors){
					this->m_ok = false;
					return false;
				}else
					std::exit(-1);
			}

			return true;
		}

		template <typename... T>
		inline void expect_and_read(const T... tt){
			if(this->expect(tt...))
				this->read_next_token();
		}

		template <typename... T>
		inline void debug_expect([[maybe_unused]] const T... tt)const{
#			ifdef DEBUG
				this->expect(tt...);
#			endif
		}

		inline void read_next_token(){
			this->m_lexer.read_next_token(this->m_token);
		}
};

#endif	// PARSER_HPP