#version 150
// ^ Change this to version 130 if you have compatibility issues

// Refer to the lambert shader files for useful comments

in vec4 fs_Col;
in vec3 fs_UV;

out vec4 out_Col;

uniform sampler2D u_Texture;

void main()
{
    vec3 col = texture(u_Texture, fs_UV.xy).rgb;
    col *= vec3(1, 0.5, 0.5);
    out_Col = vec4(col, 1);
}
