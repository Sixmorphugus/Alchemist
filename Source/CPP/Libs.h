// Copyright Chris Sixsmith 2020.

// Includes libraries.

#if IS_WEB
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <emscripten.h>
#include <emscripten/html5.h>
#include <unistd.h>
#else
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#endif

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <unordered_map>
#include <cassert>
#include <memory>
#include <filesystem>
#include <algorithm>
#include <functional>

using namespace std;