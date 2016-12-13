#version 150
// ^ Change this to version 130 if you have compatibility issues

// Refer to the lambert shader files for useful comments

uniform mat4 u_Model;
uniform mat4 u_lightSpaceMatrix;
uniform vec4 u_Color;

in vec4 vs_coord; //vertices coordinates


void main()
{

    //built-in things to pass down the pipeline
    gl_Position = u_lightSpaceMatrix * u_Model * vec4(vec3(vs_coord), 1.0f);

}
