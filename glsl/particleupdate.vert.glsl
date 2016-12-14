#version 150
// ^ Change this to version 130 if you have compatibility issues

// Refer to the lambert shader files for useful comments

in vec4 vs_Pos;

out vec4 oPos;

void main()
{
    fs_Col = vs_Col;
    vec4 modelposition = u_Model * vs_Pos;

    //built-in things to pass down the pipeline
    gl_Position = u_ViewProj * modelposition;

}
