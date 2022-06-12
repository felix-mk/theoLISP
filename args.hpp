#ifndef ARGS_HPP
#define ARGS_HPP

#include <string>

struct CommandLineArguments{
	bool dump_ast = false;
	bool dump_sym_table = false;

	bool pythonify = false;
	bool interactive_mode = false;

	bool try_recovery_from_syntax_errors = false;

	std::string filename{};
};

// Global command line arguments
static CommandLineArguments args{};

#endif	// ARGS_HPP