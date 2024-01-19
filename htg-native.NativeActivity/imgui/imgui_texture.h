#ifndef __htgnative_imgui_texture_h_impl__
#define __htgnative_imgui_texture_h_impl__

#include <stb_image.h>
#include "../utils.hpp"
#include "imgui.h"
#include "imgui_format.h"
namespace ImGui {
	struct ImageTexture {
		ImTextureID texid = 0;
		size_t width, height;
        static bool LoadTextureFromDataImpl(unsigned char* image_data, int width, int height, ImageTexture* out_data);
        // Assume we're using OpenGL
        static ImageTexture LoadTextureFromFile(const char* filename);
        // Assume we're using OpenGL
        static ImageTexture* LoadTextureFromFStream(std::ifstream __s);

        operator ImTextureID();
        static ImageTexture* LoadTextureFromMemory(const char* data, size_t len);
        static ImageTexture* LoadTextureFromArray(unsigned char* pixels, int width, int height);
	};

};



#endif