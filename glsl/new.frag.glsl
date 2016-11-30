#version 150
// ^ Change this to version 130 if you have compatibility issues

// This is a fragment shader. If you've opened this file first, please
// open and read lambert.vert.glsl before reading on.
// Unlike the vertex shader, the fragment shader actually does compute
// the shading of geometry. For every pixel in your program's output
// screen, the fragment shader is run for every bit of geometry that
// particular pixel overlaps. By implicitly interpolating the position
// data passed into the fragment shader by the vertex shader, the fragment shader
// can compute what color to apply to its pixel based on things like vertex
// position, light position, and vertex color.

uniform vec4 u_Color; // The color with which to render this instance of geometry.

uniform sampler2D u_texture; //The texture
uniform sampler2D u_texture_normal_map;

in vec2 fs_UV;
// These are the interpolated values out of the rasterizer, so you can't know
// their specific values without knowing the vertices that contributed to them
in vec4 fs_Nor;
in vec4 fs_LightVec1;
in vec4 fs_LightVec2;

out vec4 out_Col; // This is the final output color that you will see on your
                  // screen for the pixel that is currently being processed.

void main()
{
    // Material base color (before shading)
        vec4 diffuseColor;
        //texture color
        diffuseColor = texture(u_texture, fs_UV);
        //normal_MAP's normal
        vec3 normal = texture(u_texture_normal_map, fs_UV).rgb;
        normal = normalize(normal * 2.0f - 1.0f);
        vec4 fs_normal = vec4(normal, 1.0f);
        // Calculate the diffuse term for Lambert shading
        float diffuseTerm = dot(normalize(fs_normal), normalize(fs_LightVec1));
        // Avoid negative lighting values
        diffuseTerm = clamp(diffuseTerm, 0, 1);

        float ambientTerm = 0.2;

        float lightIntensity = 0.5 * diffuseTerm + ambientTerm;   //Add a small float value to the color multiplier
                                                            //to simulate ambient lighting. This ensures that faces that are not
                                                            //lit by our point light are not completely black.
        diffuseTerm = dot(normalize(fs_Nor), normalize(fs_LightVec2));
        diffuseTerm = clamp(diffuseTerm, 0, 1);

        lightIntensity += 0.5 * diffuseTerm;
        // Compute final shaded color
        out_Col = vec4(diffuseColor.rgb * lightIntensity, diffuseColor.a);
        //out_Col = texture(u_texture, vec2(0.99f,0.99f));
}
