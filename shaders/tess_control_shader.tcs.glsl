// tessellation control shader
#version 410 core

// specify number of control points per patch output
// this value controls the size of the input and output arrays
layout (vertices=3) out;

uniform vec4 TessLevelFactors;

void main()
{
    // ----------------------------------------------------------------------
    // invocation zero controls tessellation levels for the entire patch
    if (gl_InvocationID == 0)
    {
        gl_TessLevelOuter[0] = TessLevelFactors[0];
        gl_TessLevelOuter[1] = TessLevelFactors[1];
        gl_TessLevelOuter[2] = TessLevelFactors[2];
        gl_TessLevelOuter[3] = 1;

        gl_TessLevelInner[0] = TessLevelFactors[3];
        gl_TessLevelInner[1] = TessLevelFactors[3];
    }
}