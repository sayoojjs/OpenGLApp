#version 330 core

in vec2 TexCoord;
out vec4 colour;

uniform sampler2D iconTexture;
uniform vec3 iconColour;

void main()
{
    vec4 texColour = texture(iconTexture, TexCoord);
    if (texColour.a < 0.1) discard; 
    colour = vec4(texColour.rgb * iconColour, texColour.a);
}