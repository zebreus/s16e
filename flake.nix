{
  description = "Strassenbahnanzeige";

  inputs = {
    flake-utils.url = "github:numtide/flake-utils";
    nixpkgs.url = "github:nixos/nixpkgs";
    nixpkgs-esp-dev = {
      url = "github:mirrexagon/nixpkgs-esp-dev";
      inputs.nixpkgs.follows = "nixpkgs";
    };
  };

  outputs =
    {
      self,
      nixpkgs,
      flake-utils,
      nixpkgs-esp-dev,
    }:
    flake-utils.lib.eachDefaultSystem (
      system:
      let
        pkgs = import nixpkgs {
          inherit system;
          overlays = [
            (final: prev: { esp-idf-esp32c3 = nixpkgs-esp-dev.packages.${system}.esp-idf-esp32c3; })
          ];
        };
      in
      {
        name = "s16e";

        devShell = pkgs.mkShellNoCC {
          buildInputs = [
            pkgs.esp-idf-esp32c3
            (pkgs.clang-tools.override { enableLibcxx = false; })
            pkgs.glibc_multi.dev
          ];

          shellHook = ''
            RISCV_COMPILER=$(which riscv32-esp-elf-g++)
            RISCV_COMPILER_DIR=$(dirname $(dirname $RISCV_COMPILER))
            cat <<EOF > controller-esp-idf/.clangd
            CompileFlags:
              Add: [
                -I${pkgs.esp-idf-esp32c3}/include,
                -I$RISCV_COMPILER_DIR/riscv32-esp-elf/include,
                -I$RISCV_COMPILER_DIR/lib/gcc/riscv32-esp-elf/13.2.0/include,
                -I${pkgs.glibc_multi.dev}/include ]
              Remove: [ -fstrict-volatile-bitfields, -fno-tree-switch-conversion ]
              Compiler: $RISCV_COMPILER
            EOF
          '';
        };

        formatter = pkgs.nixfmt-rfc-style;
      }
    );
}
