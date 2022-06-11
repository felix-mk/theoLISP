#ifndef TOKEN_POSITION_HPP
#define TOKEN_POSITION_HPP

#include <ostream>

#include <cstdint>

class TokenPosition{
	private:
		uint32_t m_col;
		uint32_t m_line;

	public:
		explicit TokenPosition(const uint32_t col = 1, const uint32_t line = 1)noexcept:
			m_col{col}, m_line{line}{
		}

		inline uint32_t col()const noexcept{
			return this->m_col;
		}

		inline uint32_t line()const noexcept{
			return this->m_line;
		}

		void inc_col()noexcept{
			++this->m_col;
		}

		void inc_line()noexcept{
			++this->m_line;
			this->m_col = 0;
		}

		friend std::ostream& operator<< (std::ostream& os, const TokenPosition pos)noexcept{
			return os << "col: " << pos.col() << ", ln: " << pos.line();
		}
};

#endif	// TOKEN_POSITION_HPP