#include "photon_core.h"
#include "photon_opengl.h"
#include "photon_texture.h"

#include <SOIL.h>
#include <map>
#include <physfs.h>

namespace photon{

namespace texture{

std::map<std::string, GLuint> textures = std::map<std::string, GLuint>();

GLuint Load(const std::string &filename){
    GLuint texture;
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
                texture = SOIL_load_OGL_texture_from_memory(buffer, length, SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_INVERT_Y | SOIL_FLAG_MIPMAPS);

                free(buffer);

                if(texture > 0){
                    PrintToLog("INFO: Loaded texture \"%s\"", filename.c_str());
                    textures[filename] = texture;
                    return texture;
                }
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
