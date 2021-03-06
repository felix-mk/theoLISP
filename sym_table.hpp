#ifndef SYM_TABLE_HPP
#define SYM_TABLE_HPP

#include <unordered_map>

#include <string>
#include <ostream>

#include "types.hpp"

class SymbolTable{
	private:
		std::unordered_map<std::string, IntType> m_table;

	public:
		explicit SymbolTable(): m_table{}{
		}

		inline IntType get_or_insert(const std::string& symbol){
			return this->m_table[symbol];
		}

		inline void update(const std::string& symbol, const IntType value){
			this->m_table[symbol] = value;
		}

		void dump(std::ostream& os)const{
			os << "SymTable:\n";
			for(const auto& entry : this->m_table)
				os << ' ' << entry.first << ": " << entry.second << '\n';
		}
};

#endif	// SYM_TABLE_HPP