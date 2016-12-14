#version 150
// ^ Change this to version 130 if you have compatibility issues

// Refer to the lambert shader files for useful comments

//uniform sampler2D u_texture;
in vec4 fs_Col;
in vec2 fs_Uv;
uniform float u_textureflag;
uniform sampler2D u_FlatTexture;

out vec4 out_Col;


void main()
{
    // Copy the color; there is no shading.
    if(u_textureflag==0)
        out_Col = fs_Col;
    else
    {
        vec3 color = texture(u_FlatTexture, fs_Uv).rgb;
        out_Col=vec4(color,1.0);
//        out_Col=vec4(0,0,0,1);
    }

}
