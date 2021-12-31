{ pkgs ? import <nixpkgs> { }
, lib ? pkgs.lib
, stdenv ? pkgs.stdenv
}:

stdenv.mkDerivation {
  pname = "libnereon";
  version = "unstable";

  src = ./.;

  buildInputs = with pkgs; [
    openssl
    libucl
  ];

  patches = [
    ./nix/no-download.patch
  ];

  preConfigurePhases = ''
    preparePrerequisites
  '';

  preparePrerequisites = ''
    mkdir -p build
    cp -r ${pkgs.libucl.src} build/libucl
    chmod -R +w build/libucl
  '';

  buildPhase = ''
    make
  '';

  doCheck = true;

  checkPhase = ''
    make test
  '';

  nativeBuildInputs = with pkgs; [
    autoconf
    automake
    cmake
    libtool
    pkg-config
  ];

  outputs = [ "out" "dev" ];

  meta = with lib; {
    homepage = "https://github.com/riboseinc/libnereon";
    description = "a multi-configuration parser library implemented in C";
    license = licenses.bsd2;
    platforms = platforms.all;
    maintainers = with maintainers; [ ribose-jeffreylau ];
  };
}
