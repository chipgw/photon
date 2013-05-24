#include "photon_core.h"
#include "photon_texture.h"

#include <map>
#include <SDL_image.h>
#include <physfs.h>

namespace photon{

namespace texture{

std::map<std::string, GLuint> textures = std::map<std::string, GLuint>();

GLuint Load(const std::string &filename){
    if(textures.count(filename)){
        return textures[filename];
    }else{
        if(!filename.empty() && PHYSFS_exists(filename.c_str())){
            auto fp = PHYSFS_openRead(filename.c_str());
            intmax_t length = PHYSFS_fileLength(fp);
            if(length > 0){
                uint8_t *buffer = (uint8_t*) malloc(length);

                PHYSFS_read(fp, buffer, 1, length);

                PHYSFS_close(fp);

                SDL_RWops *rw = SDL_RWFromMem(buffer, length);
                SDL_Surface *image = IMG_Load_RW(rw, 1);

                free(buffer);

                if(image == nullptr){
                    PrintToLog("ERROR: texture loading failed! %s", IMG_GetError());
                    return 0;
                }

                SDL_Surface *converted = SDL_ConvertSurface(image, SDL_AllocFormat(SDL_PIXELFORMAT_RGBA8888), 0);
                SDL_FreeSurface(image);

                GLuint texture;
                glGenTextures(1, &texture);
                glBindTexture(GL_TEXTURE_2D, texture);
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, converted->w, converted->h, 0, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8, converted->pixels);

                glGenerateMipmap(GL_TEXTURE_2D);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

                static const float border_color[] = { 0.0f, 0.0f, 0.0f, 0.0f };
                glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, border_color);

                SDL_FreeSurface(converted);

                PrintToLog("INFO: Loaded texture \"%s\"", filename.c_str());
                textures[filename] = texture;
                return texture;
            }

        }else{
            PrintToLog("ERROR: Unable to load texture: \"%s\" does not exist!", filename.c_str());
        }
    }
    PrintToLog("ERROR: Unable to open texture: \"%s\", unknown error.", filename.c_str());
    return 0;
}

void GarbageCollect(){
    for(auto tex : textures){
        glDeleteTextures(1, &tex.second);
    }
    textures.clear();
}
}

}
