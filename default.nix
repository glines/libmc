{ stdenv, fetchgit, cmake, assimp, glew, glm, SDL2, SDL2_image, doxygen, emscripten, cacert, nodejs, cgdb }:

stdenv.mkDerivation rec {
  name = "libmc-${version}";
  version = "git";

  buildInputs = [
    cmake assimp glew glm SDL2 SDL2_image doxygen emscripten cacert nodejs cgdb
  ];
}

