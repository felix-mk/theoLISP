#ifndef AST_NODE_HPP
#define AST_NODE_HPP

#include <list>
#include <memory>

#include <sstream>
#include <string_view>

#include <cstdint>

#include "types.hpp"
#include "sym_table.hpp"
#include "token_position.hpp"

class BaseNode{
	protected:
		TokenPosition m_pos;

	public:
		explicit BaseNode(const TokenPosition& pos)noexcept: m_pos{pos}{
		}

		inline const TokenPosition& pos()const noexcept{
			return this->m_pos;
		}

		virtual IntType eval(SymbolTable& sym_table)const noexcept = 0;

		virtual void pythonify(std::ostream& os, const uint16_t depth)const noexcept = 0;

		virtual void dump(std::ostream& os, const uint16_t depth)const noexcept = 0;

		virtual ~BaseNode()noexcept = default;

	protected:
		static void indent_n(std::ostream& os, const uint16_t n)noexcept{
			for(uint16_t i = 0; i < n; ++i)
				os << "  ";
		}

		static void dump_placeholder(std::ostream& os, const uint16_t depth)noexcept{
			os << ' ';
			for(uint16_t i = 0; i < depth - 1; ++i)
				os << "|  ";

			os << "+--";
		}
};

class ErrorNode: public BaseNode{
	public:
		explicit ErrorNode(const TokenPosition& pos)noexcept: BaseNode{pos}{
		}

		IntType eval(SymbolTable& /*sym_table*/)const noexcept override{
			return IntType{};
		}

		void pythonify(std::ostream& os, const uint16_t depth)const noexcept override{
			BaseNode::indent_n(os, depth);
			os << "assert false\n";
		}

		void dump(std::ostream& os, const uint16_t depth)const noexcept override{
			BaseNode::dump_placeholder(os, depth);
			os << "ErrorNode[" << this->m_pos << "]\n";
		}
};

class IntNode: public BaseNode{
	private:
		const IntType m_value;

	public:
		IntNode(const IntType value, const TokenPosition& pos)noexcept:
			BaseNode{pos}, m_value{value}{
		}

		IntType eval(SymbolTable& /*sym_table*/)const noexcept override{
			return this->m_value;
		}

		void pythonify(std::ostream& os, const uint16_t /*depth*/)const noexcept override{
			os << this->m_value;
		}

		void dump(std::ostream& os, const uint16_t depth)const noexcept override{
			BaseNode::dump_placeholder(os, depth);
			os << "IntNode[" << this->m_value << ", " << this->m_pos << "]\n";
		}
};

class VarNode: public BaseNode{
	private:
		const std::string_view m_var_name;

	public:
		VarNode(const std::string_view& var_name, const TokenPosition& pos)noexcept:
			BaseNode{pos}, m_var_name{var_name}{
		}

		IntType eval(SymbolTable& sym_table)const noexcept override{
			return sym_table.get_or_insert(std::string{this->m_var_name});
		}

		void pythonify(std::ostream& os, const uint16_t /*depth*/)const noexcept override{
			os << this->m_var_name;
		}

		void dump(std::ostream& os, const uint16_t depth)const noexcept override{
			BaseNode::dump_placeholder(os, depth);
			os << "VarNode[" << this->m_var_name << ", " << this->m_pos << "]\n";
		}
};

class ArithNode: public BaseNode{
	protected:
		std::unique_ptr<BaseNode> m_param1;
		std::unique_ptr<BaseNode> m_param2;

	public:
		ArithNode(
				BaseNode* const param1,
				BaseNode* const param2,
				const TokenPosition& pos
			)noexcept:
				BaseNode{pos},
				m_param1{param1},
				m_param2{param2}{
		}
};

class AddNode: public ArithNode{
	public:
		AddNode(
				BaseNode* const param1,
				BaseNode* const param2,
				const TokenPosition& pos
			)noexcept:
				ArithNode{param1, param2, pos}{
		}

		IntType eval(SymbolTable& sym_table)const noexcept override{
			return this->m_param1->eval(sym_table) + this->m_param2->eval(sym_table);
		}

		void pythonify(std::ostream& os, const uint16_t depth)const noexcept override{
			os << '(';
			this->m_param1->pythonify(os, depth);
			os << " + ";
			this->m_param2->pythonify(os, depth);
			os << ')';
		}

		void dump(std::ostream& os, const uint16_t depth)const noexcept override{
			BaseNode::dump_placeholder(os, depth);
			os << "AddNode[" << this->m_pos << "]:\n";

			this->m_param1->dump(os, depth + 1);
			this->m_param2->dump(os, depth + 1);
		}
};

class SubNode: public ArithNode{
	public:
		SubNode(
				BaseNode* const param1,
				BaseNode* const param2,
				const TokenPosition& pos
			)noexcept:
				ArithNode{param1, param2, pos}{
		}

		IntType eval(SymbolTable& sym_table)const noexcept override{
			return this->m_param1->eval(sym_table) - this->m_param2->eval(sym_table);
		}

		void pythonify(std::ostream& os, const uint16_t depth)const noexcept override{
			os << '(';
			this->m_param1->pythonify(os, depth);
			os << " - ";
			this->m_param2->pythonify(os, depth);
			os << ')';
		}

		void dump(std::ostream& os, const uint16_t depth)const noexcept override{
			BaseNode::dump_placeholder(os, depth);
			os << "SubNode[" << this->m_pos << "]:\n";

			this->m_param1->dump(os, depth + 1);
			this->m_param2->dump(os, depth + 1);
		}
};

class MulNode: public ArithNode{
	public:
		MulNode(
				BaseNode* const param1,
				BaseNode* const param2,
				const TokenPosition& pos
			)noexcept:
				ArithNode{param1, param2, pos}{
		}

		IntType eval(SymbolTable& sym_table)const noexcept override{
			return this->m_param1->eval(sym_table) * this->m_param2->eval(sym_table);
		}

		void pythonify(std::ostream& os, const uint16_t depth)const noexcept override{
			this->m_param1->pythonify(os, depth);
			os << " * ";
			this->m_param2->pythonify(os, depth);
		}

		void dump(std::ostream& os, const uint16_t depth)const noexcept override{
			BaseNode::dump_placeholder(os, depth);
			os << "MulNode[" << this->m_pos << "]:\n";

			this->m_param1->dump(os, depth + 1);
			this->m_param2->dump(os, depth + 1);
		}
};

class InstrNode: public BaseNode{
	public:
		explicit InstrNode(const TokenPosition& pos)noexcept: BaseNode{pos}{
		}
};

class AssignNode: public InstrNode{
	private:
		std::string_view m_var_name;
		std::unique_ptr<BaseNode> m_value;

	public:
		AssignNode(
				std::string_view var_name,
				BaseNode* const value,
				const TokenPosition& pos
			)noexcept:
				InstrNode{pos}, m_var_name{var_name}, m_value{value}{
		}

		IntType eval(SymbolTable& sym_table)const noexcept override{
			sym_table.update(
				std::string{this->m_var_name},
				this->m_value->eval(sym_table)
			);

			return IntType{};
		}

		void pythonify(std::ostream& os, const uint16_t depth)const noexcept override{
			BaseNode::indent_n(os, depth);
			os << this->m_var_name << " = ";
			this->m_value->pythonify(os, depth);
			os << '\n';
		}

		void dump(std::ostream& os, const uint16_t depth)const noexcept override{
			BaseNode::dump_placeholder(os, depth);
			os << "SetNode[" << this->m_var_name << ", " << this->m_pos << "]:\n";

			this->m_value->dump(os, depth + 1);
		}
};

class IfNode: public InstrNode{
	private:
		std::unique_ptr<BaseNode> m_cond;
		std::unique_ptr<BaseNode> m_if_branch;
		std::unique_ptr<BaseNode> m_else_branch;

	public:
		IfNode(
				BaseNode* const cond,
				BaseNode* const if_branch,
				BaseNode* const else_branch,
				const TokenPosition& pos
			)noexcept:
				InstrNode{pos},
				m_cond{cond},
				m_if_branch{if_branch},
				m_else_branch{else_branch}{
		}

		IntType eval(SymbolTable& sym_table)const noexcept override{
			if(this->m_cond->eval(sym_table) > IntType{})
				this->m_if_branch->eval(sym_table);
			else
				this->m_else_branch->eval(sym_table);

			return IntType{};
		}

		void pythonify(std::ostream& os, const uint16_t depth)const noexcept override{
			BaseNode::indent_n(os, depth);
			os << "if ";
			this->m_cond->pythonify(os, depth);
			os << " > 0:\n";
			this->m_if_branch->pythonify(os, depth + 1);
			BaseNode::indent_n(os, depth);
			os << "else:\n";
			this->m_else_branch->pythonify(os, depth + 1);
			os << '\n';
		}

		void dump(std::ostream& os, const uint16_t depth)const noexcept override{
			BaseNode::dump_placeholder(os, depth);
			os << "FuncIfNode[" << this->m_pos << "]:\n";

			this->m_cond->dump(os, depth + 1);
			this->m_if_branch->dump(os, depth + 1);
			this->m_else_branch->dump(os, depth + 1);
		}
};

class WhileNode: public InstrNode{
	private:
		std::unique_ptr<BaseNode> m_cond;
		std::unique_ptr<BaseNode> m_body;

	public:
		WhileNode(
				BaseNode* const cond,
				BaseNode* const body,
				const TokenPosition& pos
			)noexcept:
				InstrNode{pos}, m_cond{cond}, m_body{body}{
		}

		IntType eval(SymbolTable& sym_table)const noexcept override{
			while(this->m_cond->eval(sym_table) > IntType{})
				this->m_body->eval(sym_table);

			return IntType{};
		}

		void pythonify(std::ostream& os, const uint16_t depth)const noexcept override{
			BaseNode::indent_n(os, depth);
			os << "while ";
			this->m_cond->pythonify(os, depth);
			os << " > 0:\n";
			this->m_body->pythonify(os, depth + 1);
			os << '\n';
		}

		void dump(std::ostream& os, const uint16_t depth)const noexcept override{
			BaseNode::dump_placeholder(os, depth);
			os << "WhileNode[" << this->m_pos << "]:\n";

			this->m_cond->dump(os, depth + 1);
			this->m_body->dump(os, depth + 1);
		}
};

class InstrListNode: public BaseNode{
	private:
		std::list<std::unique_ptr<BaseNode>> m_list;

	public:
		explicit InstrListNode(const TokenPosition& pos)noexcept:
			BaseNode{pos}, m_list{}{
		}

		void add(BaseNode* const node)noexcept{
			this->m_list.emplace_back(node);
		}

		IntType eval(SymbolTable& sym_table)const noexcept override{
			for(const auto& elem : this->m_list)
				elem->eval(sym_table);

			return IntType{};
		}

		void pythonify(std::ostream& os, const uint16_t depth)const noexcept override{
			for(const auto& elem : this->m_list)
				elem->pythonify(os, depth);

			if(0 == this->m_list.size()){
				BaseNode::indent_n(os, depth);
				os << "pass\n";
			}
		}

		void dump(std::ostream& os, const uint16_t depth)const noexcept override{
			BaseNode::dump_placeholder(os, depth);
			os << "InstrListNode[" << this->m_pos << "]:\n";

			for(const auto& elem : this->m_list)
				elem->dump(os, depth + 1);
		}
};

#endif	// AST_NODE_HPP