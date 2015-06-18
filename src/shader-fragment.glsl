#version 300 es

in highp vec2 f_texcoords;
uniform sampler2D texture;

out highp vec4 color;

void main(void)
{
    color = texture2D(texture, f_texcoords);
    color = vec4(color.z, color.y, color.x, color.t);
}

