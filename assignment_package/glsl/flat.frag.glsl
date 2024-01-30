#version 150
// ^ Change this to version 130 if you have compatibility issues

// Refer to the lambert shader files for useful comments

in vec4 fs_Col;
in vec2 fs_UV;

out vec4 out_Col;
out vec2 out_UV;

void main()
{
    // Copy the color; there is no shading.
    out_UV = fs_UV;
    out_Col = fs_Col;
}
