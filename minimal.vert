
#version 330

// Note this vertex shader executes once per vertex in the array
layout (location = 0) in vec3 a_vertex;

uniform mat4 transform;

out vec3 pos;

void main(void)
{
    // gl_position is a built in variable to store vertex position
    // It has 4 coordinates for reasons we'll see later on.
	gl_Position = transform * vec4(a_vertex, 1.0);
	pos = a_vertex;
}
