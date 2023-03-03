{
  description = "A flake for building clibsedml";

  inputs.nixpkgs.url = github:NixOS/nixpkgs/nixos-22.11;

  outputs = { self, nixpkgs }: {

    packages.x86_64-linux.default =
      with import nixpkgs { system = "x86_64-linux"; };
      stdenv.mkDerivation {
        pname = "clibsedml";
        version = "0.1.3";
        nativeBuildInputs = [ autoconf automake libtool ];
        buildInputs = [ libxml2 ];
        src = self;
        configurePhase = "autoreconf -i -f && ./configure --prefix=$out";
        buildPhase = "make";
        # doCheck = true; # submodule sed-ml is unavailable
        checkPhase = "make check";
        installPhase = "make install";
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
        buildPhase = "make";
        # doCheck = true; # submodule sed-ml is unavailable
        checkPhase = "make check";
        installPhase = "make install";
      };
  };
}
