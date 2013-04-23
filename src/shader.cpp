#include "photon_core.h"
#include "photon_texture.h"

#include <physfs.h>
#include <libxml/parser.h>

namespace photon{

namespace opengl{

GLuint LoadAndCompileShader(const char *filename, GLenum shader_type){
    GLuint shader = glCreateShader(shader_type);

    PHYSFS_File *file;
    long length;
    char *source;

    file = PHYSFS_openRead(filename);
    if (!file){
        PrintToLog("ERROR: unable to open shader file \"%s\"", filename);
        return 0;
    }

    length = PHYSFS_fileLength(file);
    source = (char*)malloc(length+1);

    PHYSFS_read(file, source, 1, length);
    PHYSFS_close(file);
    source[length] = 0;

    glShaderSource(shader, 1, (const GLchar**)&source, 0);

    glCompileShader(shader);

    free(source);

    int isCompiled,maxLength;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

    if(maxLength > 1){
        char *infoLog = (char *)malloc(maxLength);

        glGetShaderInfoLog(shader, maxLength, &maxLength, infoLog);

        if(isCompiled == GL_FALSE) {
            PrintToLog("ERROR: failed to compile \"%s\"!\n\tLog: %s", filename, infoLog);
            free(infoLog);

            return 0;
        }else{
            PrintToLog("INFO: succesfully compiled \"%s\".\n\tLog: %s", filename, infoLog);
            free(infoLog);
        }
    }else{
        if(isCompiled == GL_FALSE) {
            PrintToLog("ERROR: failed to compile \"%s\"! No log availible.", filename);

            return 0;
        }else{
            PrintToLog("INFO: succesfully compiled \"%s\". No log availible.", filename);
        }
    }
    return shader;
}

int LinkShaderProgram(photon_shader &shader){
    shader.program = glCreateProgram();

    for(GLuint shader_object : shader.shader_objects){
        glAttachShader(shader.program, shader_object);
    }

    glLinkProgram(shader.program);

    int isLinked, maxLength;
    glGetProgramiv(shader.program, GL_LINK_STATUS, (int *)&isLinked);
    glGetProgramiv(shader.program, GL_INFO_LOG_LENGTH, &maxLength);

    if(maxLength > 1){
        char*infoLog = (char *)malloc(maxLength);

        glGetProgramInfoLog(shader.program, maxLength, &maxLength, infoLog);

        if(isLinked == GL_FALSE){
            PrintToLog("ERROR: failed to link shader program!\n\tLog: %s", infoLog);
        }else{
            PrintToLog("INFO: succesfully linked shader.\n\tLog: %s", infoLog);
        }
        free(infoLog);
    }else{
        if(isLinked == GL_FALSE){
            PrintToLog("ERROR: failed to link shader program! No log availible.");
        }else{
            PrintToLog("INFO: succesfully linked shader. No log availible.");
        }
    }
    shader.is_valid = isLinked;
    return isLinked;
}

void DeleteShader(photon_shader &shader){
    glDeleteProgram(shader.program);
    for(GLuint shader_object : shader.shader_objects){
        glDeleteShader(shader_object);
    }
    shader.is_valid = false;
}

GLuint ParseShaderObjectXML(photon_shader &shader, xmlDocPtr doc, xmlNodePtr node, GLenum shader_type, const std::string &xml_filename){
    xmlChar *filenameptr = xmlGetProp(node, (const xmlChar *)"file");
    std::string filename = (char*)filenameptr;

    // if the file does not exist and is not an absolute path try checking relative to XML file.
    if(!PHYSFS_exists(filename.c_str()) && filename[0] != '/'){
        filename = xml_filename.substr(0,xml_filename.find_last_of('/')+1).append(filename);
        PrintToLog("INFO: file \"%s\" does not exist, trying path relative to XML file: \"%s\"", filenameptr, filename.c_str());
    }

    GLuint shader_object = LoadAndCompileShader(filename.c_str(), shader_type);

    shader.shader_objects.push_back(shader_object);

    xmlFree(filenameptr);
    return shader_object;
}

photon_shader LoadShaderXML(const std::string &filename){
    photon_shader shader;
    if(PHYSFS_exists(filename.c_str())){
        PHYSFS_File *file;
        long length;
        char *xml_buffer;

        file = PHYSFS_openRead(filename.c_str());
        if(!file){
            PrintToLog("ERROR: unable to open XML Shader file \"%s\"", filename.c_str());
            return shader;
        }

        length = PHYSFS_fileLength(file);
        xml_buffer = (char*)malloc(length);

        PHYSFS_read(file, xml_buffer, 1, length);
        PHYSFS_close(file);

        xmlDocPtr doc = xmlParseMemory(xml_buffer, length);

        if(doc == nullptr) {
            PrintToLog("ERROR: Unable to load XML Shader: Document not parsed successfully!");
            return shader;
        }

        xmlNodePtr root = xmlDocGetRootElement(doc);
        if(root == nullptr) {
            PrintToLog("ERROR: Unable to load XML Shader: empty document!");
            xmlFreeDoc(doc);
            return shader;
        }
        if(xmlStrcmp(root->name, (const xmlChar *) "photon_shader")) {
            PrintToLog("ERROR: Unable to load XML Shader: root node not photon_shader!");
            xmlFreeDoc(doc);
            return shader;
        }

        xmlNodePtr node = root->xmlChildrenNode;
        while(node != nullptr) {
            if(xmlStrEqual(node->name, (const xmlChar *)"vertex_shader")){
                ParseShaderObjectXML(shader, doc, node, GL_VERTEX_SHADER, filename);
            }else if(xmlStrEqual(node->name, (const xmlChar *)"fragment_shader")){
                ParseShaderObjectXML(shader, doc, node, GL_FRAGMENT_SHADER, filename);
            }
            node = node->next;
        }
        LinkShaderProgram(shader);

        glUseProgram(shader.program);

        node = root->xmlChildrenNode;
        while(node != nullptr) {
            if((xmlStrEqual(node->name, (const xmlChar *)"input"))){
                xmlChar *input_name = xmlGetProp(node, (const xmlChar *)"name");
                xmlChar *input_type = xmlGetProp(node, (const xmlChar *)"type");

#ifndef NDEBUG
                PrintToLog("DEBUG: input name %s type %s", input_name, input_type);
#endif

                if(xmlStrEqual(input_type, (const xmlChar *)"location")){
                    glBindAttribLocation(shader.program, PHOTON_VERTEX_LOCATION_ATTRIBUTE, (const GLchar *)input_name);
                }else if(xmlStrEqual(input_type, (const xmlChar *)"uv")){
                    glBindAttribLocation(shader.program, PHOTON_VERTEX_UV_ATTRIBUTE, (const GLchar *)input_name);
                }
                xmlFree(input_name);
                xmlFree(input_type);
            }else if((xmlStrEqual(node->name, (const xmlChar *)"texture2D"))){
                xmlChar *uniform_name = xmlGetProp(node, (const xmlChar *)"name");
                GLuint texuniform = glGetUniformLocation(shader.program, (const GLchar *)uniform_name);

                xmlChar *texture_type = xmlGetProp(node, (const xmlChar *)"type");

#ifndef NDEBUG
                PrintToLog("DEBUG: uniform name %s type %s", uniform_name, texture_type);
#endif

                if(xmlStrEqual(texture_type, (const xmlChar *)"color")){
                    glUniform1i(texuniform, PHOTON_TEXTURE_UNIT_COLOR - GL_TEXTURE0);
                }else if(xmlStrEqual(texture_type, (const xmlChar *)"light")){
                    glUniform1i(texuniform, PHOTON_TEXTURE_UNIT_LIGHT - GL_TEXTURE0);
                }
                xmlFree(uniform_name);
                xmlFree(texture_type);
            }
            node = node->next;
        }

        free(xml_buffer);
        xmlFreeDoc(doc);
    }else{
        PrintToLog("ERROR: Unable to load XML Shader: file \"%s\" does not exist!", filename.c_str());
    }
    // TODO - maybe store loaded shaders in a list of some sort and return a reference?
    return shader;
}

}
}
