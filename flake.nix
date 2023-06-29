{
  description = "A flake for building clibsedml";

  inputs.nixpkgs.url = github:NixOS/nixpkgs/nixos-23.05;

  inputs.sed-ml = {
    url = github:SED-ML/sed-ml/44511c318bcf93c43256ac201905ace1f7675e4c;
    flake = false;
  };

  outputs = { self, nixpkgs, sed-ml }: let

    allSystems = [ "x86_64-linux" "aarch64-linux" "x86_64-darwin" "aarch64-darwin" ];

    forAllSystems = f: nixpkgs.lib.genAttrs allSystems (system: f (import nixpkgs { inherit system; }));

  in {

    packages = forAllSystems (pkgs: with pkgs; {

      default = stdenv.mkDerivation {
        pname = "clibsedml";
        version = "0.1.3";

        nativeBuildInputs = [ autoreconfHook ];

        buildInputs = [ libxml2 ];

        src = self;

        preCheck = ''
          ln -s ${sed-ml} sed-ml
        '';

        doCheck = true;

        enableParallelBuilding = true;
      };

    });

  };
}
