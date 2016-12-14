#version 150
// ^ Change this to version 130 if you have compatibility issues

// Refer to the lambert shader files for useful comments

in vec4 vs_Pos;

uniform float u_Time;

void main()
{
    gl_Position = vs_Pos - vec4(0, u_Time * 10, 0, 0);
}
