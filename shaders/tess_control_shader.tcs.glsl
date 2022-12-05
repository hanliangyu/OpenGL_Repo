// tessellation control shader
#version 410 core

// specify number of control points per patch output
// this value controls the size of the input and output arrays
layout (vertices=3) out;

layout (location = 0) in float OL1;
layout (location = 1) in float OL2;
layout (location = 2) in float OL3;
layout (location = 3) in float IL1;

in vec3 vPosition[];
out vec3 tcPosition[];

void main()
{
    // ----------------------------------------------------------------------
    // pass attributes through
    // gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
    tcPosition[gl_InvocationID] = vPosition[gl_InvocationID];

    // ----------------------------------------------------------------------
    // invocation zero controls tessellation levels for the entire patch
    if (gl_InvocationID == 0)
    {
        gl_TessLevelOuter[0] = OL1;
        gl_TessLevelOuter[1] = OL2;
        gl_TessLevelOuter[2] = OL3;
        gl_TessLevelOuter[3] = 1;

        gl_TessLevelInner[0] = IL1;
        gl_TessLevelInner[1] = IL1;
    }
}