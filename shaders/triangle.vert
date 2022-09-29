#version 450

layout (location = 0) in vec3 vPosition;
layout (location = 1) in vec3 vNormal;
layout (location = 2) in vec3 vColor;
layout (location = 0) out vec3 outColor;

layout (push_constant) uniform constants
{
    vec4 data;
    mat4 render_matrix;
} PushConstants;

void main()
{
    //    const vec3 positions[3] = vec3[3](
    //        vec3(1.0f, 1.0f, 0.0f),
    //        vec3(-1.0f, 1.0f, 0.0f),
    //        vec3(0.0f, -1.0f, 0.0f)
    //    );
    //
    //    const vec3 colors[3] = vec3[3](
    //        vec3(1.0f, 0.0f, 0.0f),
    //        vec3(0.0f, 1.0f, 0.0f),
    //        vec3(0.0f, 0.0f, 1.0f)
    //    );

    //    gl_Position = vec4(positions[gl_VertexIndex], 1.0f);
    //    outColor = colors[gl_VertexIndex];
    gl_Position = PushConstants.render_matrix * vec4(vPosition, 1.0f);
    outColor = vNormal;
}