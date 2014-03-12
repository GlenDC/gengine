solution "gengine"
   configurations { "Debug", "Release", "DebugEmscripten", "ReleaseEmscripten" }

   project "gengine"
      kind "ConsoleApp"
      language "C++"
      files { "../src/**.h", "../src/**.cpp" }

      includedirs { "../src/kernel", "../src/core", "../src/window", "../src/graphics", "../src/input" }

      links { "lua" }

      buildoptions { "-std=c++11" }

      configuration "Debug*"
         defines { "DEBUG" }
         flags { "Symbols" }
         targetname "gengined"
 
      configuration "Release*"
         defines { "NDEBUG" }
         flags { "Optimize" }
         targetname "gengine"

      configuration "*Emscripten"
         defines { "EMSCRIPTEN" }
         libdirs { "../deps/emscripten/lib" }
         includedirs { "../deps/emscripten/include" }
         targetsuffix ".bc"
         postbuildcommands { "emcc $(TARGET) -o gengine.html" }

      configuration "not *Emscripten"
         links { "SDL2", "GL" }
