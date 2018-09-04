
#version 330

#ifdef GL_ES
precision mediump float;
#endif

// The final colour we will see at this location on screen
//uniform float u_time;
uniform vec4 colour;

in vec3 pos;
out vec4 fragColour;

void main(){

    float horizontal = step(0.05 * abs(sin(pos.x * 40)), pos.y);

    if(horizontal == 0.0){
        fragColour = vec4(1.0, 1.0, 0.9, 1.0);
    } else {
        fragColour = colour;
    }

}
