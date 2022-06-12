#include <string>

#include <fstream>
#include <sstream>
#include <iostream>

#include "ast.hpp"
#include "parser.hpp"
#include "sym_table.hpp"
#include "types.hpp"

#include "arg_parser.hpp"

static void interpret(const std::string& code)noexcept{
	Parser parser{code};
	SymbolTable sym_table{};

	const Ast ast = parser.parse();
	const IntType res = ast.eval(sym_table);

	std::ostringstream oss{};
	if(args.dump_ast)
		ast.dump(oss << '\n');

	if(args.dump_sym_table)
		sym_table.dump(oss << '\n');

	if(args.pythonify)
		ast.pythonify(oss << '\n');

	oss << "\n-> " << res << '\n';
	std::cout << oss.str();
}

static void run_interactive_mode()noexcept{
	while(true){
		std::string code{};

		std::cin.sync();
		std::getline(std::cin, code, ';');

		interpret(code);
	}
}

static void run_filename_mode()noexcept{
	std::string code{};
	std::ifstream file{args.filename};

	if(!file){
		std::cerr << "error: Invalid filename \'" << args.filename << "\'.\n";
		return;
	}

	std::getline(file, code, '\0');
	interpret(code);
}

int main(int argc, const char* argv[]){
	parse_args(argc, argv);
	if(args.interactive_mode)
		run_interactive_mode();
	else
		run_filename_mode();

	return 0;
}