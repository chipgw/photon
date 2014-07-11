#include "photon_core.h"

#include <ft2build.h>
#include FT_FREETYPE_H
#include <physfs.h>
#include <glm/gtc/type_ptr.hpp>

#define FONT_SIZE 32
#define FONT_FIRST_CHAR 32
#define FONT_LAST_CHAR 128

namespace photon{

namespace gui{
struct character_info {
    glm::vec2 advance;

    float width;
    float rows;
    float left;
    float top;

    float x;
};

struct character_atlas{
    character_info characters[FONT_LAST_CHAR+1];

    int width;

    GLuint texture;
};

// TODO - store this in a better location.
character_atlas main_atlas;

void InitFreeType(){
    PrintToLog("INFO: Initializing FreeType.");
    FT_Library ft;
    FT_Face face;

    if(FT_Init_FreeType(&ft)) {
        PrintToLog("ERROR: Could not init FreeType library!");
        return;
    }

    // TODO - load from some configuration file...
    const char *font_filename = "/Orbitron Bold.otf";

    PHYSFS_File *file;
    intmax_t length;
    uint8_t *font_buffer;

    file = PHYSFS_openRead(font_filename);
    if(!file){
        PrintToLog("ERROR: unable to open Font File: \"%s\"", font_filename);
        return;
    }

    length = PHYSFS_fileLength(file);
    font_buffer = new uint8_t[length];

    PHYSFS_read(file, font_buffer, 1, length);
    PHYSFS_close(file);

    FT_Error err = FT_New_Memory_Face(ft, font_buffer, length, 0, &face);

    if(err){
        PrintToLog("ERROR: Could not load font \"%s\"! error code %x!", font_filename, err);
        delete[] font_buffer;
        return;
    }

    FT_Set_Pixel_Sizes(face, 0, FONT_SIZE);

    FT_GlyphSlot g = face->glyph;
    int width = 0;
    int height = 0;

    for(unsigned char c = FONT_FIRST_CHAR; c <= FONT_LAST_CHAR; c++) {
        if(FT_Load_Char(face, c, FT_LOAD_RENDER)) {
            PrintToLog("WARNING: Loading character %c failed!", c);
            continue;
        }

        width += g->bitmap.width + 1;
        height = std::max(height, g->bitmap.rows);
    }

    main_atlas.width = width;

    glGenTextures(1, &main_atlas.texture);
    glBindTexture(GL_TEXTURE_2D, main_atlas.texture);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    std::vector<GLubyte> empty_image(width * height, 0);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, width, height, 0, GL_ALPHA, GL_UNSIGNED_BYTE, &empty_image[0]);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    uint32_t x = 0;

    for(unsigned char i = FONT_FIRST_CHAR; i <= FONT_LAST_CHAR; i++) {
        if(FT_Load_Char(face, i, FT_LOAD_RENDER))
            continue;

        glTexSubImage2D(GL_TEXTURE_2D, 0, x, 0, g->bitmap.width, g->bitmap.rows, GL_ALPHA, GL_UNSIGNED_BYTE, g->bitmap.buffer);

        main_atlas.characters[i].advance.x = g->advance.x >> 6;
        main_atlas.characters[i].advance.y = g->advance.y >> 6;

        main_atlas.characters[i].width = g->bitmap.width;
        main_atlas.characters[i].rows = g->bitmap.rows;

        main_atlas.characters[i].left = g->bitmap_left;
        main_atlas.characters[i].top = g->bitmap_top;

        main_atlas.characters[i].x = (float)x / (float)width;

        x += g->bitmap.width + 1;
    }

    PrintToLog("INFO: Created character atlas size %ix%i", width,height);

    FT_Done_Face(face);
    delete[] font_buffer;

    // TODO - perhaps leave this open for loading other fonts and close in a GarbageCollect() function?
    FT_Done_FreeType(ft);
}

void RenderText(glm::vec2 position, glm::vec2 scale, glm::vec4 color, bool center, const char *format,...) {
    va_list args;
    va_start(args, format);

    std::string out;

    // TODO - maybe calculate this somehow?
    out.reserve(512);

    vsnprintf(&out[0], out.capacity(), format, args);
    va_end(args);

    RenderText(position, scale, color, center, out);
}

void RenderText(glm::vec2 position, glm::vec2 scale, glm::vec4 color, bool center, const std::string &text) {
    scale /= FONT_SIZE;
    glBindTexture(GL_TEXTURE_2D, main_atlas.texture);

    opengl::SetColorGUI(color);

    std::vector<glm::vec2> origin{position};

    for(const unsigned char *c = (const unsigned char*)text.c_str(); *c; c++) {
        if(*c == '\n'){
            origin.push_back(glm::vec2(position.x, position.y - scale.y * FONT_SIZE * origin.size()));
        }else if(center){
            origin.back() -= main_atlas.characters[*c].advance * scale * 0.5f;
        }
    }

    position = origin[0];
    uint8_t current_line = 0;

    std::vector<glm::vec2> verts;
    std::vector<glm::vec2> uv;

    for(const unsigned char *c = (const unsigned char*)text.c_str(); *c; c++) {
        if(*c == '\n'){
            position = origin[++current_line];
            continue;
        }
        character_info &character = main_atlas.characters[*c];
        float x =  position.x + character.left * scale.x;
        float y = position.y + character.top * scale.y;
        float w = character.width * scale.x;
        float h = character.rows * scale.y;

        // Advance the cursor to the start of the next character
        position += character.advance * scale;

        // Skip glyphs that have no pixels
        if(!w || !h)
            continue;

        uv.push_back(glm::vec2(character.x, 0.0f));
        verts.push_back(glm::vec2(x, y));

        uv.push_back(glm::vec2(character.x + character.width / main_atlas.width, 0.0f));
        verts.push_back(glm::vec2(x + w, y));

        uv.push_back(glm::vec2(character.x + character.width / main_atlas.width, character.rows / FONT_SIZE));
        verts.push_back(glm::vec2(x + w, y - h));

        uv.push_back(glm::vec2(character.x + character.width / main_atlas.width, character.rows / FONT_SIZE));
        verts.push_back(glm::vec2(x + w, y - h));

        uv.push_back(glm::vec2(character.x, character.rows / FONT_SIZE));
        verts.push_back(glm::vec2(x, y - h));

        uv.push_back(glm::vec2(character.x, 0.0f));
        verts.push_back(glm::vec2(x, y));
    }

    glVertexAttribPointer(PHOTON_VERTEX_LOCATION_ATTRIBUTE, 2, GL_FLOAT, GL_FALSE, 0, glm::value_ptr(verts[0]));
    glVertexAttribPointer(PHOTON_VERTEX_UV_ATTRIBUTE, 2, GL_FLOAT, GL_FALSE, 0, glm::value_ptr(uv[0]));

    glDrawArrays(GL_TRIANGLES, 0, verts.size());
}

float GetTextWidth(const std::string &text, glm::vec2 scale, uint32_t start_pos, int32_t end_pos){
    if(start_pos > text.size()){
        return 0.0f;
    }
    if(end_pos < 0 || end_pos > (int32_t)text.size()){
        end_pos = text.size();
    }

    scale /= FONT_SIZE;

    glm::vec2 size(0.0f);
    for(int32_t c = start_pos; c < end_pos; c++){
        size += main_atlas.characters[(unsigned char)text[c]].advance * scale;
    }
    return size.x;
}

std::pair<int32_t, int32_t> GetTextLimits(const std::string &text, float desired_width, glm::vec2 scale, std::string::size_type end_pos){
    scale /= FONT_SIZE;

    if(end_pos > text.size()){
        end_pos = text.size();
    }

    glm::vec2 size(0.0f);
    for(std::string::size_type c = std::min(end_pos, text.size()); c > 0; c--){
        size += main_atlas.characters[(unsigned char)text[c]].advance * scale;
        if(size.x >= desired_width){
            return std::pair<int32_t, int32_t>(c + 1, end_pos);
        }
    }

    size = glm::vec2(0.0f);
    // if going from the end did not reach the full width go from start.
    for(std::string::size_type c = 0; c < text.length(); c++){
        size += main_atlas.characters[(unsigned char)text[c]].advance * scale;
        if(size.x >= desired_width){
            return std::pair<int32_t, int32_t>(0, c - 1);
        }
    }
    return std::pair<int32_t, int32_t>(0, text.size());
}

}

}
