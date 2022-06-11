#ifndef AST_HPP
#define AST_HPP

#include <memory>

#include "ast_node.hpp"
#include "sym_table.hpp"
#include "types.hpp"

class Ast{
	private:
		std::unique_ptr<BaseNode> m_root;

	public:
		Ast(BaseNode* const root)noexcept: m_root{root}{
		}

		IntType eval(SymbolTable& sym_table)const noexcept{
			this->m_root->eval(sym_table);
			return sym_table.get_or_insert("result");
		}

		void pythonify(std::ostream& os)const noexcept{
			this->m_root->pythonify(os << "Python:\n", 0);
		}

		void dump(std::ostream& os)const noexcept{
			this->m_root->dump(os << "Ast:\n", 1);
		}
};

#endif	// AST_HPP