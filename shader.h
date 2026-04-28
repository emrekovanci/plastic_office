#ifndef SHADER_H
#define SHADER_H

#include <stdbool.h>

typedef struct Shader Shader;
struct Shader {
    unsigned int id;
};

bool shader_load(Shader *s, const char *vert_path, const char *frag_path);
void shader_destroy(Shader *s);

void shader_bind(const Shader *s);
void shader_unbind(void);

void shader_set_int(const Shader *s, const char *name, int value);
void shader_set_float(const Shader *s, const char *name, float value);
void shader_set_mat4(const Shader *s, const char *name, const float *value);

#endif
