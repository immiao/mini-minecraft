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
uniform int u_Time;

uniform sampler2D u_texture; //The texture
uniform sampler2D u_texture_normal_map;
uniform sampler2D u_cosine_power_map;

//// These are the interpolated values out of the rasterizer, so you can't know
//// their specific values without knowing the vertices that contributed to them
in vec4 fs_LightVec1;

in vec3 FragPos;
in vec2 fs_UV;
in vec3 TangentLightPos;
in vec3 TangentViewPos;
in vec3 TangentFragPos;
flat in int IsFluid;

out vec4 out_Col; // This is the final output color that you will see on your
                  // screen for the pixel that is currently being processed.

void main()
{
        vec2 new_UV;
        if(IsFluid == 1){
            //WATER or LAVA
            new_UV.x = fs_UV.x + float(u_Time) * 1.0f/16.0f / 150.0f;
            new_UV.y = fs_UV.y;
        }
        else if(IsFluid == 2){
            new_UV.x = fs_UV.x + 1.0f / 16.0f - float(u_Time) * 1.0f/16.0f / 150.0f;
            new_UV.y = fs_UV.y;
        }
        else if(IsFluid == 3){
            //Down
            new_UV.x = fs_UV.x;
            new_UV.y = fs_UV.y + 1.0f/16.0f - float(u_Time) * 1.0f/16.0f * 1.0f / 150.0f;
        }
        else if(IsFluid == 4){
            //Up
            new_UV.x = fs_UV.x;
            new_UV.y = fs_UV.y + float(u_Time) * 1.0f/16.0f * 1.0f / 150.0f;
        }
        else{
            new_UV = fs_UV;
        }
    // Obtain normal from normal map in range [0,1]
        vec3 normal = texture(u_texture_normal_map, new_UV).rgb;
        // Transform normal vector to range [-1,1]
        normal = normalize(normal * 2.0 - 1.0);  // this normal is in tangent space

        // Get diffuse color
        vec3 color = texture(u_texture, new_UV).rgb;
        vec3 color2 = texture(u_cosine_power_map, new_UV).rgb;
        float cosine_power = color2.x * 256.0f;
        // Ambient
        vec3 ambient = 0.2 * color;
        // Diffuse
        vec3 lightDir = normalize(vec3(fs_LightVec1));
        float diff = max(dot(lightDir, normal), 0.0);
        vec3 diffuse = 0.8 * diff * color;
        // Specular
        vec3 viewDir = normalize(TangentViewPos - TangentFragPos);
        //vec3 reflectDir = reflect(-lightDir, normal);
        vec3 halfwayDir = normalize(lightDir + viewDir);
        float spec = max(pow(dot(normal, halfwayDir), cosine_power * cosine_power), 0.0);
        vec3 specular = vec3(0.3) * spec;

        out_Col = vec4(ambient + diffuse + specular, 1.0f);
}
