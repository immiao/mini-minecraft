#version 150
// ^ Change this to version 130 if you have compatibility issues

//This is a vertex shader. While it is called a "shader" due to outdated conventions, this file
//is used to apply matrix transformations to the arrays of vertex data passed to it.
//Since this code is run on your GPU, each vertex is transformed simultaneously.
//If it were run on your CPU, each vertex would have to be processed in a FOR loop, one at a time.
//This simultaneous transformation allows your program to run much faster, especially when rendering
//geometry with millions of vertices.

uniform mat4 u_Model;       // The matrix that defines the transformation of the
                            // object we're rendering. In this assignment,
                            // this will be the result of traversing your scene graph.

uniform mat4 u_ModelInvTr;  // The inverse transpose of the model matrix.
                            // This allows us to transform the object's normals properly
                            // if the object has been non-uniformly scaled.

uniform mat4 u_ViewProj;    // The matrix that defines the camera's transformation.
                            // We've written a static matrix for you to use for HW2,
                            // but in HW3 you'll have to generate one yourself
uniform vec3 u_ViewPos;

//Using chunks to render
in vec4 vs_coord; //vertices coordinates
in vec4 vs_nor;   //vertices surface normal
in vec2 vs_UV;    //vertices texture uv
in vec3 vs_tangent;
in vec3 vs_bitangent;
in int vs_IsFluid;

out vec4 fs_LightVec1;       // The direction in which our virtual light lies, relative to each vertex. This is implicitly passed to the fragment shader.

out vec3 FragPos;
out vec2 fs_UV;
out vec3 TangentLightPos;
out vec3 TangentViewPos;
out vec3 TangentFragPos;
flat out int IsFluid;

const vec4 lightDir = vec4(6,5,4,0);  // The direction of our virtual light, which is used to compute the shading of
                                        // the geometry in the fragment shader.

const vec3 lightPos = vec3(3,7,5);

void main()
{
    gl_Position = u_ViewProj * u_Model * vs_coord;
    FragPos = vec3(u_Model * vs_coord);
    fs_UV = vs_UV;
    IsFluid = vs_IsFluid;

    mat3 normalMatrix = transpose(mat3(u_ModelInvTr));
    vec3 T = normalize(normalMatrix * vs_tangent);
    vec3 B = normalize(normalMatrix * vs_bitangent);
    vec3 N = normalize(normalMatrix * vec3(vs_nor));
    mat3 TBN = transpose(mat3(T, B, N));
    TangentLightPos = TBN * lightPos;
    TangentViewPos  = TBN * u_ViewPos;
    TangentFragPos  = TBN * FragPos;

    fs_LightVec1 = lightDir;
}
