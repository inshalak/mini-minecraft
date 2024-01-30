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

uniform sampler2D u_Texture; // MS2: The texture to be read from by this shader
uniform int u_Time;  // MS2: Time to animate

// These are the interpolated values out of the rasterizer, so you can't know
// their specific values without knowing the vertices that contributed to them
in vec4 fs_Pos;
in vec4 fs_Nor;
in vec4 fs_LightVec;
in vec4 fs_Col;
in vec3 fs_UV;

out vec4 out_Col; // This is the final output color that you will see on your
                  // screen for the pixel that is currently being processed.



/*
Epsilon Equality ----
Float epsilon = 0.001;

abs(Val - 10.f) <= epsilon

  */

float worleyNoise(vec2 uv, float time) {
    float maxDist = 0.0;
    vec2 maxDistVec = vec2(0.0);

    for (int xo = -1; xo <= 1; ++xo) {
        for (int yo = -1; yo <= 1; ++yo) {
            vec2 offset = vec2(float(xo), float(yo));
            vec2 pos = floor(uv) + offset;
            vec2 randomVec = vec2(
                fract(sin(dot(pos, vec2(127.1, 311.7))) * 43758.5453 + time),
                fract(sin(dot(pos, vec2(269.5, 183.3))) * 43758.5453 + time)
            );

            vec2 delta = randomVec - fract(uv);
            float dist = length(delta);

            if (dist > maxDist) {
                maxDist = dist;
                maxDistVec = delta;
            }
        }
    }

    return maxDist;
}

void main()
{
        // MS2: CHANGED to account for texture - Material base color (before shading)
        // ignore QT compiler warnings, glsl will compile
    vec2 uv = fs_UV.xy;
    vec4 diffuseColor;

    if (fs_UV.z == 1) {
        // Generate worley noise texture
        float worley = worleyNoise(fs_UV.xy * 10 , u_Time*0.01);

        // Apply offset
        float maxOffset = 0.0625f * 2;
        float offset = maxOffset * worley;
        vec2 offsetUV = vec2(fs_UV.x + offset, fs_UV.y);

        // Sample texture
        diffuseColor = texture(u_Texture, offsetUV);
    }
    else {
        diffuseColor = texture(u_Texture, fs_UV.xy);
        if (diffuseColor.a < 0.5)
            discard;
    }
/*
    if (fs_UV.z == 1) {
           float max = 0.0625f * 2;
           float n = 10.0f;
           float singleStep = max / n;

           int speed = 2;
           if (u_Time % (100 * speed) < (10 * speed)) {
               singleStep *= 1;
           } else if (u_Time % (100 * speed) < (20 * speed))  {
               singleStep *= 2;
           } else if (u_Time % (100 * speed) < (30 * speed))  {
               singleStep *= 3;
           } else if (u_Time % (100 * speed) < (40 * speed))  {
               singleStep *= 4;
           } else if (u_Time % (100 * speed) < (50 * speed))  {
               singleStep *= 5;
           } else if (u_Time % (100 * speed) < (60 * speed))  {
               singleStep *= 6;
           } else if (u_Time % (100 * speed) < (70 * speed))  {
               singleStep *= 7;
           } else if (u_Time % (100 * speed) < (80 * speed))  {
               singleStep *= 8;
           } else if (u_Time % (100 * speed) < (90 * speed))  {
               singleStep *= 9;
           } else {
               singleStep *= 10;
           }
            diffuseColor = texture(u_Texture, vec2(uv.x + singleStep, uv.y) );
    }
    else {
          diffuseColor = texture(u_Texture, vec2(uv.x, uv.y) );
    }
    */
    float diffuseTerm = dot(normalize(fs_Nor), normalize(fs_LightVec));
        // Avoid negative lighting values
        diffuseTerm = clamp(diffuseTerm, 0, 1);

        float ambientTerm = 0.2;

        float lightIntensity = diffuseTerm + ambientTerm;   //Add a small float value to the color multiplier
                                                            //to simulate ambient lighting. This ensures that faces that are not
                                                            //lit by our point light are not completely black.

        // Compute final shaded color
       out_Col = vec4(diffuseColor.rgb * lightIntensity, diffuseColor.a);
}
