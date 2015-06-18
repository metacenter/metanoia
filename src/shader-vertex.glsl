#version 300 es

in vec2 vertices;
in vec2 texcoords;
uniform ivec2 screen_size;

out vec2 f_texcoords;

void main(void)
{
    mat2 view_matrix = mat2(2.0/float(screen_size.x),          0.0,
                                    0.0,              -2.0/float(screen_size.y));

    vec2 translation_vector = vec2(-1.0, 1.0);

    gl_Position = vec4(view_matrix * vertices + translation_vector, 0.0, 1.0);
    f_texcoords = vec2(vertices.x != 0.0, vertices.y != 0.0);
}

