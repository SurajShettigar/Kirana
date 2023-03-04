#version 460
#extension GL_GOOGLE_include_directive: enable

#include "common/globals.glsl"

layout (location = 0) rayPayloadInEXT PathtracePayload payload;

hitAttributeEXT vec2 hitAttrib;

void main()
{
    payload.hitDistance = gl_HitTEXT;
    payload.primitiveID = gl_PrimitiveID;
    payload.instanceID = gl_InstanceID;
    payload.geometryIndex = gl_GeometryIndexEXT ;
    payload.instanceCustomIndex = gl_InstanceCustomIndexEXT;
    payload.barycentricCoords = hitAttrib;
    payload.objectToWorldMat = gl_ObjectToWorldEXT;
    payload.worldToObjectMat = gl_WorldToObjectEXT;
}