#version 330 core
in vec3 ourColor;
in vec2 TexCoord;

out vec4 color;

uniform sampler2D myTexture1;

void main()
{
    // Linearly interpolate between both textures (second texture is only slightly combined)
    color = texture(myTexture1, TexCoord);
}
