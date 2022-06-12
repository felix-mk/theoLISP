#ifndef ARG_PARSER_HPP
#define ARG_PARSER_HPP

#include <string>

#include <cstdlib>

#include "args.hpp"

static void print_ussage_and_exit(const char* const prog_name)noexcept{
	std::clog << "usage: " << prog_name
			  << " [<filename>] [--interactive] [--dump-ast] [--dump-sym] [--pythonify] [--try-recovery-from-syntax-errors]\n";

	std::exit(0);
}

static void parse_args(const int argc, const char* const argv[])noexcept{
	bool file_specified = false;
	for(int arg_idx = 1; arg_idx < argc; ++arg_idx){
		const std::string arg = argv[arg_idx];

		if(arg == "--interactive")
			args.interactive_mode = true;
		else if(arg == "--dump-ast")
			args.dump_ast = true;
		else if(arg == "--dump-sym")
			args.dump_sym_table = true;
		else if(arg == "--pythonify")
			args.pythonify = true;
		else if(arg == "--try-recovery-from-syntax-errors")
			args.try_recovery_from_syntax_errors = true;
		else if(!file_specified){
			args.filename = arg;
			file_specified = true;
		}else
			print_ussage_and_exit(*argv);
	}

	// Either in interactive mode and no file
	// or a file but not in interactive mode.
	if(file_specified == args.interactive_mode)
		print_ussage_and_exit(*argv);
}

#endif	// ARG_PARSER_HPP