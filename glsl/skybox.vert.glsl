#version 150

in vec3 vs_Pos;

uniform mat4 u_ViewProj;

out vec3 TexCoord0;

void main()
{
    vec4 WVP_Pos = u_ViewProj * vec4(vs_Pos, 1.0);
    gl_Position = WVP_Pos.xyww;
    TexCoord0 = vs_Pos;
    //TexCoord0.z = -TexCoord0.z;
}
