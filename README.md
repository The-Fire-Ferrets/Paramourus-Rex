# Compiling and Running
To play the game on the department lab machines, a build script has been 
provided.  Just run:

    ./scripts/init

If necessary, give the script execute permissions using:

    chmod +x scripts/init

This process should also work on other Linux installations.

For Windows, you will need to set the environment variables
`CMAKE_MODULE_PATH`, `INCLUDE`, and `LIB` to point to your SFML install
location.  If needed, a local copy is provided in the repository.  Should you
choose to use it, be aware that you may need to remove it, and replace it 
with a freshly unpacked copy from `assets/SFML-2.3.1.tgz`.

# Cleaning Up
To remove files created from CMake and building the game, run:

    make clean && ./scripts/cmake_cleanup
