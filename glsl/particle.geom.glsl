#version 150 compatibility

layout(points) in;
layout(triangle_strip, max_vertices=4) out;
uniform mat4 u_ViewProj;
uniform vec3 u_cameraPos;

void main()
{
    vec3 f = normalize(u_cameraPos - gl_in[0].gl_Position.xyz);
    vec3 up = vec3(0, 1, 0);
    vec3 right = cross(f, up) * 0.1;
    vec3 top = cross(right, f);

    gl_Position = u_ViewProj * vec4(gl_in[0].gl_Position.xyz - top + right, 1.0);
    EmitVertex();
    gl_Position = u_ViewProj * vec4(gl_in[0].gl_Position.xyz - top - right, 1.0);
    EmitVertex();
    gl_Position = u_ViewProj * vec4(gl_in[0].gl_Position.xyz + top + right, 1.0);
    EmitVertex();
    gl_Position = u_ViewProj * vec4(gl_in[0].gl_Position.xyz + top - right, 1.0);
    EmitVertex();

    EndPrimitive();
}
