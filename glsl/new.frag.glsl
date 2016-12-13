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
uniform int u_DayTime;
uniform int u_OpenDNcycle;

uniform sampler2D u_texture; //The texture
uniform sampler2D u_texture_normal_map;
uniform sampler2D u_cosine_power_map;
uniform sampler2D u_depth_map;

//// These are the interpolated values out of the rasterizer, so you can't know
//// their specific values without knowing the vertices that contributed to them
in vec4 fs_LightVec1;

in vec3 FragPos;
in vec2 fs_UV;
in vec3 TangentViewPos;
in vec3 TangentFragPos;
in vec4 FragPosLightSpace;
in vec4 fs_Nor;

flat in int IsFluid;

out vec4 out_Col; // This is the final output color that you will see on your
                  // screen for the pixel that is currently being processed.

float ShadowCalculation(vec4 fragPosLightSpace, float dot_n_l)
{
    // perform perspective divide
        vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
        // Transform to [0,1] range
        projCoords = projCoords * 0.5 + 0.5;
        // Get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
        float closestDepth = texture(u_depth_map, projCoords.xy).r;
//      add a bias margin
        float bias = 0.001;
        // Get depth of current fragment from light's perspective
        float currentDepth = projCoords.z;
        // Check whether current frag pos is in shadow
        float shadow = currentDepth > closestDepth  ? 1.0 : 0.0;

        vec2 texelSize = 1.0 / textureSize(u_depth_map, 0);
        for(int x = -1; x <= 1; ++x)
        {
            for(int y = -1; y <= 1; ++y)
            {
                float pcfDepth = texture(u_depth_map, projCoords.xy + vec2(x, y) * texelSize).r;
                shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
            }
        }
        shadow /= 9.0;
        return shadow;
}

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
        vec3 ambient = 0.18 * color;
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

        // Calculate shadow
        float shadow = ShadowCalculation(FragPosLightSpace, dot(normal, lightDir));

        float Lightintense;
        if(u_DayTime < 625 && u_OpenDNcycle == 1){
            //Day to Night
            Lightintense = 1.0f - 0.4f / 625.0f * u_DayTime;
        }
        else if(u_DayTime >= 625 && u_OpenDNcycle == 1){
            //Night to Day
            Lightintense = 0.6f + 0.4f / 625.0f * (u_DayTime - 625);
        }
        else{
            Lightintense = 1;
        }
        out_Col = Lightintense * vec4(ambient + (1.0- 0.7 * shadow) * diffuse + (1.0- 0.7 * shadow) * specular, 1.0f);
}
