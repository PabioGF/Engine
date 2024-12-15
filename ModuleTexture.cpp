#include "Globals.h"
#include "Application.h"
#include "ModuleProgram.h"
#include "ModuleWindow.h"
#include "ModuleTexture.h"
#include "ModuleRenderExercise.h"
#include "ModuleInput.h"
#include "ModuleOpenGL.h"
#include "ModuleDebugDraw.h"
#include "GL/glew.h"
#include "SDL.h"
#include "MathGeoLib.h"
#include "DirectXTex/DirectXTex/DirectXTex/DirectXTex.h"
#include <string>
#include <comdef.h>



/**
 * Constructor for the ModuleTexture class.
 */
ModuleTexture::ModuleTexture() {

}

/**
 * Destructor for the ModuleTexture class.
 */
ModuleTexture::~ModuleTexture() {

}

/**
 * Loads a texture from a file path using DirectXTex.
 *
 * @param file_path The path of the texture file to be loaded.
 * @return A ScratchImage containing the loaded texture.
 */
DirectX::ScratchImage ModuleTexture::LoadTexture(const std::string& file_path) {
    DirectX::ScratchImage image;
    LOG("File path received in LoadTexture: %s", file_path.c_str());
    std::wstring wide_file_path(file_path.begin(), file_path.end());

    HRESULT hr = DirectX::LoadFromDDSFile(
        wide_file_path.c_str(),
        DirectX::DDS_FLAGS_NONE,
        nullptr,
        image
    );

    if (FAILED(hr)) {
        hr = DirectX::LoadFromTGAFile(
            wide_file_path.c_str(),
            nullptr,
            image
        );

        if (FAILED(hr)) {
            hr = DirectX::LoadFromWICFile(
                wide_file_path.c_str(),
                DirectX::WIC_FLAGS_NONE,
                nullptr,
                image
            );

            if (FAILED(hr)) {
                _com_error err(hr);
                LOG("Failed to load texture from WIC file: %s. Error: %s", file_path.c_str(), err.ErrorMessage());
                return {};
            }
        }
    }

    LOG("Texture loaded successfully: %s", file_path);
    return image;

}

/**
 * Creates an OpenGL texture from a DirectX ScratchImage.
 *
 * @param scratch_image The ScratchImage object containing the texture data.
 * @return The ID of the created OpenGL texture.
 */
unsigned int ModuleTexture::CreateTexture(const DirectX::ScratchImage& scratch_image) {

    DirectX::TexMetadata metadata = scratch_image.GetMetadata();
    unsigned int texture_id;

    glGenTextures(1, &texture_id);
    glBindTexture(GL_TEXTURE_2D, texture_id);

    GLint internal_format;
    GLenum format;
    GLenum type;

    switch (metadata.format) {
    case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
    case DXGI_FORMAT_R8G8B8A8_UNORM:
        internal_format = GL_RGBA8;
        format = GL_RGBA;
        type = GL_UNSIGNED_BYTE;
        break;
    case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB:
    case DXGI_FORMAT_B8G8R8A8_UNORM:
        internal_format = GL_RGBA8;
        format = GL_BGRA;
        type = GL_UNSIGNED_BYTE;
        break;
    case DXGI_FORMAT_B5G6R5_UNORM:
        internal_format = GL_RGB8;
        format = GL_BGR;
        type = GL_UNSIGNED_BYTE;
        break;
    default:
        assert(false && "Unsupported format");
    }

    if (metadata.mipLevels <= 1) {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    for (size_t i = 0; i < metadata.mipLevels; ++i) {
        const DirectX::Image* image = scratch_image.GetImage(i, 0, 0);
        glTexImage2D(
            GL_TEXTURE_2D,
            static_cast<GLint>(i),
            internal_format,
            static_cast<GLsizei>(image->width),
            static_cast<GLsizei>(image->height),
            0,
            format,
            type,
            image->pixels
        );
    }

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, scratch_image.GetMetadata().mipLevels - 1);

    glActiveTexture(GL_TEXTURE5);
    
    LOG("Texture created succesfully");
    return texture_id;
}


