include_paths = [ "../../src/math/", "../../src/tokenizer/", "../../src/uncategorized/", "../../src/", "../../src/renderer/", "../../src/scene/", "../../src/ui/", "../../src/resources/", "../../src/utility/" ]
#precompiled_headers = ["../../src/uncategorized/common.h"]
precompiled_headers = []
project_name = "AnKi"
compiler = "g++"
common_flags = ""
compiler_flags = "-c -pedantic-errors -pedantic -ansi -Wall -Wextra -W -pipe -O0 -g3 -pg `sdl-config --cflags` -D_DEBUG_ -D_TERMINAL_COLORING_ -D_PLATFORM_LINUX_"
precompiled_headers_flags = "-x c++-header"
linker_flags = "-lGL -lGLU -lSDL_image `sdl-config --static-libs` -ljpeg -lGLEW"
