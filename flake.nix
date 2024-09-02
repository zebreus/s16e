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
          # CPATH = pkgs.lib.makeSearchPathOutput "dev" "include" buildInputs;

          buildInputs = [
            pkgs.esp-idf-esp32c3
            "/nix/store/i6zxjr9akhs8s3qhqpzy3zf31i2jwrmq-riscv32-esp-elf-esp-idf-v5.3/riscv32-esp-elf"
            "/nix/store/i6zxjr9akhs8s3qhqpzy3zf31i2jwrmq-riscv32-esp-elf-esp-idf-v5.3/lib/gcc/riscv32-esp-elf/13.2.0"
            (pkgs.clang-tools.override { enableLibcxx = false; })
            pkgs.glibc_multi.dev
          ];

          shellHook = ''
            cat <<EOF > controller-esp-idf/hello_world/.clangd
            CompileFlags:
              Add: [
                -I${pkgs.esp-idf-esp32c3}/include,
                -I${"/nix/store/i6zxjr9akhs8s3qhqpzy3zf31i2jwrmq-riscv32-esp-elf-esp-idf-v5.3/riscv32-esp-elf"}/include,
                -I${"/nix/store/i6zxjr9akhs8s3qhqpzy3zf31i2jwrmq-riscv32-esp-elf-esp-idf-v5.3/lib/gcc/riscv32-esp-elf/13.2.0"}/include,
                -I${pkgs.glibc_multi.dev}/include ]
              Compiler: /nix/store/i6zxjr9akhs8s3qhqpzy3zf31i2jwrmq-riscv32-esp-elf-esp-idf-v5.3/bin/riscv32-esp-elf-g++
            EOF
          '';
        };

        formatter = pkgs.nixfmt-rfc-style;
      }
    );
}
