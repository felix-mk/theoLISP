#ifndef UTIL_HPP
#define UTIL_HPP

#include <string>
#include <string_view>

#include <sstream>
#include <ostream>

#include <memory>
#include <iterator>

#include <cstddef>

#include "types.hpp"

static const std::string_view ZERO_SV{"0"};
static const std::string_view ERR_IDENT_SV{"__error__"};

template <typename IT>
static inline std::string_view sv_from_range(const IT beg, const IT end)noexcept{
	return std::string_view{
		std::addressof(*beg),
		static_cast<size_t>(std::distance(beg, end))
	};
}

static IntType sv_to_int(const std::string_view& sv)noexcept{
	IntType res{};
	std::stringstream ss{};

	ss << sv;
	ss >> res;

	return res;

}

static inline void print_list(std::ostream& /*os*/)noexcept{
}

template <typename A>
static inline void print_list(std::ostream& os, const A a)noexcept{
	os << a;
}

template <typename A, typename... P>
static inline void print_list(std::ostream& os, const A a, const P... p)noexcept{
	print_list<A>(os, a);
	print_list<P...>(os << ", ", p...);
}

#endif	// UTIL_HPP