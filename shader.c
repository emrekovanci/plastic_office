#include "shader.h"

#include <glad/glad.h>
#include <SDL3/SDL.h>

static const char *FALLBACK_VERT_SRC =
    "#version 330 core\n"
    "layout(location = 0) in vec3 aPos;\n"
    "layout(location = 1) in vec3 aColor;\n"
    "layout(location = 2) in vec2 aTexCoord;\n"
    "uniform mat4 model;\n"
    "uniform mat4 view;\n"
    "uniform mat4 projection;\n"
    "out vec3 ourColor;\n"
    "out vec2 TexCoord;\n"
    "void main() {\n"
    "    gl_Position = projection * view * model * vec4(aPos, 1.0);\n"
    "    ourColor = aColor;\n"
    "    TexCoord = aTexCoord;\n"
    "}\n";

static const char *FALLBACK_FRAG_SRC =
    "#version 330 core\n"
    "out vec4 FragColor;\n"
    "in vec3 ourColor;\n"
    "in vec2 TexCoord;\n"
    "uniform sampler2D texture1;\n"
    "void main() {\n"
    "    FragColor = vec4(1.0, 0.0, 1.0, 1.0);\n"
    "}\n";

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
        SDL_Log("Failed to link shader program: %s", log);
        return 0;
    }
    return prog;
}

static bool shader_load_fallback(Shader *s)
{
    GLuint vert = shader_compile(GL_VERTEX_SHADER, FALLBACK_VERT_SRC);
    GLuint frag = shader_compile(GL_FRAGMENT_SHADER, FALLBACK_FRAG_SRC);
    if (!vert || !frag) {
        glDeleteShader(vert);
        glDeleteShader(frag);
        return false; // something so wrong...
    }

    s->id = shader_link(vert, frag);
    glDeleteShader(vert);
    glDeleteShader(frag);
    return s->id != 0;
}

bool shader_load(Shader *s, const char *vert_path, const char *frag_path)
{
    char *vert_src = SDL_LoadFile(vert_path, NULL);
    char *frag_src = SDL_LoadFile(frag_path, NULL);
    if (!vert_src || !frag_src) {
        SDL_Log("Failed to load shader files: %s -- using fallback shader", SDL_GetError());
        SDL_free(vert_src);
        SDL_free(frag_src);
        return shader_load_fallback(s);
    }

    GLuint vert = shader_compile(GL_VERTEX_SHADER, vert_src);
    GLuint frag = shader_compile(GL_FRAGMENT_SHADER, frag_src);
    SDL_free(vert_src);
    SDL_free(frag_src);
    if (!vert || !frag) {
        glDeleteShader(vert);
        glDeleteShader(frag);
        return shader_load_fallback(s);
    }

    s->id = shader_link(vert, frag);
    glDeleteShader(vert);
    glDeleteShader(frag);

    return (!s->id) ? shader_load_fallback(s) : true;
}

void shader_destroy(Shader *s)
{
    glDeleteProgram(s->id);
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

void shader_set_vec2(const Shader *s, const char *name, float x, float y)
{
    glUniform2f(loc(s, name), x, y);
}

void shader_set_vec3(const Shader *s, const char *name, float x, float y, float z)
{
    glUniform3f(loc(s, name), x, y, z);
}

void shader_set_vec4(const Shader *s, const char *name, float x, float y, float z, float w)
{
    glUniform4f(loc(s, name), x, y, z, w);
}

void shader_set_mat4(const Shader *s, const char *name, const float *value)
{
    glUniformMatrix4fv(loc(s, name), 1, GL_FALSE, value);
}
