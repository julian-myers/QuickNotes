{
  description = "Dev shell + packaged JUnit Console";

  inputs.nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";

  outputs = { self, nixpkgs }:
  let
    system = "x86_64-linux";
    pkgs = import nixpkgs { inherit system; };
  in {
    devShells.${system}.default = pkgs.mkShell {
      buildInputs = with pkgs; [
					gcc
					cmake
					gnumake
					clang-tools
					stdenv.cc.cc.lib
					doxygen
					gtest
					ncurses
					tomlplusplus
					sqlite
					spdlog
				];
			shellHook = ''
					build() { cmake --build build --target check; }
					export -f build
				'';
    };
  };
}
