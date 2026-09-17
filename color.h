#ifndef COLOR_H
#define COLOR_H

typedef struct Color Color;
struct Color {
    unsigned char r;
    unsigned char g;
    unsigned char b;
    unsigned char a;
};

#define WHITE   (Color){ 255, 255, 255, 255 }
#define BLACK   (Color){ 0,   0,   0,   255 }
#define RED     (Color){ 255, 0,   0,   255 }
#define GREEN   (Color){ 0,   255, 0,   255 }
#define BLUE    (Color){ 0,   0,   255, 255 }
#define MAGENTA (Color){ 255, 0,   255, 255 }

#endif
