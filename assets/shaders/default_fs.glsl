#version 330 core

out vec4 FragColor;

in vec3 ourColor;
in vec2 TexCoord;

uniform sampler2D BaseTex;

void main()
{
    vec4 c = texture(BaseTex, TexCoord);
    if (c.a < 0.99)
        discard;
    FragColor = vec4(ourColor * c.rgb, 1.0);
}
