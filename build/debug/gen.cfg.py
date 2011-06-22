sourcePaths = ["../../src"]
sourcePaths.extend(list(walkDir("../../src")))

includePaths = ["./"]
includePaths.extend(list(sourcePaths))
includePaths.extend(["../../extern/include", "../../extern/include/bullet", "/usr/include/python2.6", "/usr/include/freetype2"])

executableName = "anki"

compiler = "g++"

compilerFlags = "-DPLATFORM_LINUX -DMATH_INTEL_SIMD -DREVISION=\\\"`svnversion ../..`\\\" -c -pedantic-errors -pedantic -ansi -Wall -Wextra -W -Wno-long-long -pipe -g3 -fsingle-precision-constant -msse4"

linkerFlags = "-rdynamic -L../../extern/lib-x86-64-linux -Wl,-Bdynamic -lBulletSoftBody -lBulletDynamics -lBulletCollision -lLinearMath -lGLEW -lGLU -lGL -ljpeg -lSDL -lpng -lpython2.6 -lboost_system -lboost_python -lboost_filesystem -lboost_thread -lfreetype"
