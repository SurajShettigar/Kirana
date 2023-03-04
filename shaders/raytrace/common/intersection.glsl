#ifndef RAYTRACE_COMMON_INTERSECTION_GLSL
#define RAYTRACE_COMMON_INTERSECTION_GLSL 1


layout (buffer_reference) readonly buffer VertexData {
    Vertex v[];
};
layout (buffer_reference) readonly buffer IndexData {
    uint32_t i[];
};

layout (set = 1, binding = 0) readonly buffer _ObjectData {
    ObjectData o[];
} objBuffer;

vec3 getWorldPosition(const vec3[3] vPositions, const vec3 barycentrics, const mat4x3 objToWorldMat)
{
    const vec3 pos = vPositions[0] * barycentrics.x + vPositions[1] * barycentrics.y + vPositions[2] * barycentrics.z;
    return vec3(objToWorldMat * vec4(pos, 1.0));
}

vec3 getWorldNormal(const vec3[3] vNormals, vec3 barycentrics, const mat4x3 worldToObjMat)
{
    const vec3 normal = vNormals[0] * barycentrics.x + vNormals[1] * barycentrics.y + vNormals[2] * barycentrics.z;
    return normalize(vec3(normal * worldToObjMat));
}

IntersectionData getIntesectionData(in PathtracePayload hitInfo)
{
    ObjectData objData = objBuffer.o[hitInfo.geometryIndex + hitInfo.instanceCustomIndex];

    // Get Indices
    IndexData iBuffer = IndexData(objData.indexBufferAddress);
    uint32_t indexOffset = objData.firstIndex + (3 * hitInfo.primitiveID);
    u32vec3 indices = u32vec3(iBuffer.i[indexOffset], iBuffer.i[indexOffset + 1], iBuffer.i[indexOffset + 2]);
    indices += u32vec3(objData.vertexOffset);

    // Get Vertices
    VertexData vBuffer = VertexData(objData.vertexBufferAddress);
    Vertex vertices[3] = Vertex[3](vBuffer.v[indices.x], vBuffer.v[indices.y], vBuffer.v[indices.z]);
    vec3[3] localPositions = vec3[3](vertices[0].position, vertices[1].position, vertices[2].position);
    vec3[3] localNormals = vec3[3](vertices[0].normal, vertices[1].normal, vertices[2].normal);
    const vec3 barycentrics = vec3(1.0 - hitInfo.barycentricCoords.x - hitInfo.barycentricCoords.y, hitInfo.barycentricCoords.x, hitInfo.barycentricCoords.y);

    IntersectionData intersection;
    intersection.position = getWorldPosition(localPositions, barycentrics, hitInfo.objectToWorldMat);
    intersection.normal = getWorldNormal(localNormals, barycentrics, hitInfo.worldToObjectMat);

    getCoordinateFrame_Duff(intersection.normal, intersection.tangent, intersection.bitangent);

    intersection.materialBufferAddress = objData.materialDataBufferAddress;
    intersection.materialIndex = objData.materialDataIndex;
    return intersection;
}

#endif