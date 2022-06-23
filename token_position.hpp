#ifndef TOKEN_POSITION_HPP
#define TOKEN_POSITION_HPP

#include <ostream>

#include <cstdint>

class TokenPosition{
	private:
		uint32_t m_col;
		uint32_t m_line;

	public:
		explicit TokenPosition(const uint32_t col = 1, const uint32_t line = 1):
			m_col{col}, m_line{line}{
		}

		inline uint32_t col()const{
			return this->m_col;
		}

		inline uint32_t line()const{
			return this->m_line;
		}

		inline void inc_col(){
			++this->m_col;
		}

		inline void inc_line(){
			++this->m_line;
			this->m_col = 0;
		}

		friend inline bool operator== (const TokenPosition& a, const TokenPosition& b){
			return a.col() == b.col() && a.line() == b.line();
		}

		friend inline bool operator!= (const TokenPosition& a, const TokenPosition& b){
			return !(a == b);
		}

		friend std::ostream& operator<< (std::ostream& os, const TokenPosition pos){
			return os << "ln: " << pos.line() << ", col: " << pos.col();
		}
};

#endif	// TOKEN_POSITION_HPP