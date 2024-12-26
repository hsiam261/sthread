{
  description = "Devenv For Building the library and Running Examples";

  inputs = {
    nixpkgs.url = "github:nixos/nixpkgs/nixos-24.11";
  };

  outputs = { self, nixpkgs }:
  let
    system="x86_64-linux";
    pkgs = nixpkgs.legacyPackages."${system}";
  in
  {
    devShells."${system}".default = pkgs.mkShell {
      packages = with pkgs; [
        cmake
        gdb
      ];
    };
  };
}
