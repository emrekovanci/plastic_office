#include "shader.h"

#include <glad/glad.h>
#include <SDL3/SDL.h>

static GLint loc(const Shader *s, const char *name)
{
    return glGetUniformLocation(s->id, name);
}

static GLuint shader_compile(GLenum type, const char *src)
{
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &src, NULL);
    glCompileShader(shader);

    GLint ok = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &ok);
    if (!ok) {
        char log[512];
        glGetShaderInfoLog(shader, sizeof(log), NULL, log);
        SDL_Log("Failed to compile shader: %s", log);
        return 0;
    }
    return shader;
}

static GLuint shader_link(GLuint vert, GLuint frag)
{
    GLuint prog = glCreateProgram();
    glAttachShader(prog, vert);
    glAttachShader(prog, frag);
    glLinkProgram(prog);

    GLint ok = 0;
    glGetProgramiv(prog, GL_LINK_STATUS, &ok);
    if (!ok) {
        char log[512];
        glGetProgramInfoLog(prog, sizeof(log), NULL, log);
        SDL_Log("Failed to link shader progarm: %s", log);
        return 0;
    }
    return prog;
}

bool shader_load(Shader *s, const char *vert_path, const char *frag_path)
{
    char *vert_src = SDL_LoadFile(vert_path, NULL);
    char *frag_src = SDL_LoadFile(frag_path, NULL);
    if (!vert_src || !frag_src) {
        SDL_Log("Failed to load shader files: %s", SDL_GetError());
        SDL_free(vert_src);
        SDL_free(frag_src);
        return false;
    }

    GLuint vert = shader_compile(GL_VERTEX_SHADER, vert_src);
    GLuint frag = shader_compile(GL_FRAGMENT_SHADER, frag_src);
    SDL_free(vert_src);
    SDL_free(frag_src);
    if (!vert || !frag) {
        glDeleteShader(vert);
        glDeleteShader(frag);
        return false;
    }

    s->id = shader_link(vert, frag);
    glDeleteShader(vert);
    glDeleteShader(frag);

    return s->id != 0;
}

void shader_destroy(Shader *s)
{
    glDeleteProgram(s->id);
    s->id = 0;
}

void shader_bind(const Shader *s)
{
    glUseProgram(s->id);
}

void shader_unbind(void)
{
    glUseProgram(0);
}

void shader_set_int(const Shader *s, const char *name, int value)
{
    glUniform1i(loc(s, name), value);
}

void shader_set_float(const Shader *s, const char *name, float value)
{
    glUniform1f(loc(s, name), value);
}

void shader_set_mat4(const Shader *s, const char *name, const float *value)
{
    glUniformMatrix4fv(loc(s, name), 1, GL_FALSE, value);
}
