{ stdenv, fetchgit, cmake, assimp, glew, glm, SDL2, SDL2_image, doxygen, emscripten, cacert, nodejs, cgdb, lua, texlive, lcov, gprof2dot, libpng }:

stdenv.mkDerivation rec {
  name = "libmc-${version}";
  version = "git";

  buildInputs = [
    cmake glew glm SDL2 SDL2_image doxygen emscripten cacert nodejs cgdb lua texlive.combined.scheme-full lcov gprof2dot libpng
  ];
}

