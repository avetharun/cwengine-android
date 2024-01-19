#include "imgui_texture.h"
#include "../include/abt/cwerror.h"
using namespace ImGui;
inline bool ImGui::ImageTexture::LoadTextureFromDataImpl(unsigned char* image_data, int width, int height, ImageTexture* out_data) {
    // Create a OpenGL texture identifier
    GLuint image_texture;
    glGenTextures(1, &image_texture);
    glBindTexture(GL_TEXTURE_2D, image_texture);
    // Setup filtering parameters for display
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // This is required on WebGL for non power-of-two textures
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); // Same
    // Upload pixels into texture
#if defined(GL_UNPACK_ROW_LENGTH) && !defined(__EMSCRIPTEN__)
    glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
#endif
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data);
    out_data->texid = &image_texture;
    out_data->width = width;
    out_data->height = height;
    return true;
}

// Assume we're using OpenGL

inline ImageTexture ImGui::ImageTexture::LoadTextureFromFile(const char* filename)
{
    // Load from file
    int image_width = 0;
    int image_height = 0;
    unsigned char* image_data = stbi_load(filename, &image_width, &image_height, NULL, 4);
    if (image_data == NULL) {
        cwError::serrof("stbi_load returned invalid image data for %s!", filename);
        return {};
    }
    ImageTexture out_tex{};
    stbi_image_free(image_data);
    if (!LoadTextureFromDataImpl(image_data, image_width, image_height, &out_tex)) {
        cwError::serrof("Unable to load texture %s", filename);
        return {};
    }
    return out_tex;
}

// Assume we're using OpenGL

inline ImageTexture* ImGui::ImageTexture::LoadTextureFromFStream(std::ifstream __s)
{
    // Load from file
    int image_width = 0;
    int image_height = 0;
    unsigned char* image_data = stbi_load(alib_file_read(__s).c_str(), &image_width, &image_height, NULL, 4);
    if (image_data == NULL) {
        cwError::serrof("stbi_load returned invalid image data for stream at %p!", (void*)&__s);
        return {};
    }
    ImageTexture* _tex = alib_malloct(ImageTexture);
    stbi_image_free(image_data);
    if (!LoadTextureFromDataImpl(image_data, image_width, image_height, _tex)) {
        cwError::serrof("Unable to load texture for stream at %p", (void*)&__s);
        return {};
    }
    return _tex;
}

inline ImageTexture* ImGui::ImageTexture::LoadTextureFromMemory(const char* data, size_t len) {
    ImageTexture* _tex = alib_malloct(ImageTexture);
    int width, height, bpp;
    unsigned char* pixels = stbi_load_from_memory((stbi_uc*)data, len, &width, &height, &bpp, 4);
    LoadTextureFromDataImpl(pixels, width, height, _tex);
    return _tex;
}

inline ImageTexture* ImGui::ImageTexture::LoadTextureFromArray(unsigned char* pixels, int width, int height) {
    ImageTexture* _tex = alib_malloct(ImageTexture);
    LoadTextureFromDataImpl(pixels, width, height, _tex);
    return _tex;
}
inline ImageTexture::operator ImTextureID() { return this->texid; }
