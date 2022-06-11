#ifndef ARG_PARSER_HPP
#define ARG_PARSER_HPP

#include <string>

#include <cstdlib>

struct CommandLineArguments{
	bool dump_ast = false;
	bool dump_sym_table = false;

	bool pythonify = false;
	bool interactive_mode = false;

	std::string filename{};
};

// Global command line arguments
CommandLineArguments args{};

static void print_ussage(const char* const prog_name)noexcept{
	std::clog << "usage: " << prog_name << " [<filename>] [--dump-ast] [--dump-sym] [--pythonify] [--interactive]\n";
}

static void parse_args(const int argc, const char* const argv[])noexcept{
	bool file_specified = false;
	for(int arg_idx = 1; arg_idx < argc; ++arg_idx){
		const std::string arg = argv[arg_idx];

		if(arg == "--dump-ast")
			args.dump_ast = true;
		else if(arg == "--dump-sym")
			args.dump_sym_table = true;
		else if(arg == "--pythonify")
			args.pythonify = true;
		else if(arg == "--interactive")
			args.interactive_mode = true;
		else if(!file_specified){
			args.filename = arg;
			file_specified = true;
		}
	}

	if(!file_specified && !args.interactive_mode){
		print_ussage(*argv);
		std::exit(-1);
	}
}

#endif	// ARG_PARSER_HPP