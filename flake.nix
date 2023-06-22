{
  description = "A flake for building clibsedml";

  inputs.nixpkgs.url = github:NixOS/nixpkgs/nixos-23.05;

  inputs.sed-ml = {
    url = github:SED-ML/sed-ml/44511c318bcf93c43256ac201905ace1f7675e4c;
    flake = false;
  };

  outputs = { self, nixpkgs, sed-ml }: {

    packages.x86_64-linux.default =
      with import nixpkgs { system = "x86_64-linux"; };
      stdenv.mkDerivation {
        pname = "clibsedml";
        version = "0.1.3";
        nativeBuildInputs = [ autoconf automake libtool ];
        buildInputs = [ libxml2 ];
        src = self;
        configurePhase = "autoreconf -i -f && ./configure --prefix=$out";
        preCheck = ''
          ln -s ${sed-ml} sed-ml
        '';
        doCheck = true;
      };

    packages.aarch64-darwin.default =
      with import nixpkgs { system = "aarch64-darwin"; };
      stdenv.mkDerivation {
        pname = "clibsedml";
        version = "0.1.3";
        nativeBuildInputs = [ autoconf automake libtool ];
        buildInputs = [ libxml2 ];
        src = self;
        configurePhase = "autoreconf -i -f && ./configure --prefix=$out";
        preCheck = ''
          ln -s ${sed-ml} sed-ml
        '';
        doCheck = true;
      };
  };
}
