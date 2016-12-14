#version 150
// ^ Change this to version 130 if you have compatibility issues

// Refer to the lambert shader files for useful comments

out vec4 out_Col;

void main()
{
    // Copy the color; there is no shading.
    //out_Col = fs_Col;
    out_Col = vec4(1.0, 1.0, 1.0, 1.0);
}
