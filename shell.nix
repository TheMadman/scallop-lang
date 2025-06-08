let
	pkgs = import <nixpkgs> {};
	scallop-lang = import ./.;
in
pkgs.mkShell {
	inputsFrom = [scallop-lang];
	nativeBuildInputs = [pkgs.gdb pkgs.graphviz pkgs.doxygen];
	shellHook = ''
		export CFLAGS='-Wall -Wextra -Wshadow -fsanitize=address -fsanitize=leak -fsanitize=undefined'
	'';
}
