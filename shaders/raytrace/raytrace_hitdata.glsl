layout (buffer_reference) readonly buffer VertexData {
    Vertex v[];
};
layout (buffer_reference) readonly buffer IndexData {
    uint32_t i[];
};

vec3 getWorldPosition(const vec3[3] vPositions, const vec3 barycentrics)
{
    const vec3 pos = vPositions[0] * barycentrics.x + vPositions[1] * barycentrics.y + vPositions[2] * barycentrics.z;
    return vec3(gl_ObjectToWorldEXT * vec4(pos, 1.0));
}

vec3 getWorldNormal(const vec3[3] vNormals, vec3 barycentrics)
{
    const vec3 normal = vNormals[0] * barycentrics.x + vNormals[1] * barycentrics.y + vNormals[2] * barycentrics.z;
    return normalize(vec3(normal * gl_WorldToObjectEXT));
}

HitPoint getHitPoint(ObjectData objData, vec2 hitAttribs)
{
    // Get Indices
    IndexData iBuffer = IndexData(objData.indexBufferAddress);
    uint32_t indexOffset = objData.firstIndex + (3 * gl_PrimitiveID);
    uint32_t vertexOffset = objData.vertexOffset;
    u32vec3 indices = u32vec3(iBuffer.i[indexOffset], iBuffer.i[indexOffset + 1], iBuffer.i[indexOffset + 2]);
    indices += u32vec3(vertexOffset);

    // Get Vertices
    VertexData vBuffer = VertexData(objData.vertexBufferAddress);
    Vertex vertices[3] = Vertex[3](vBuffer.v[indices.x], vBuffer.v[indices.y], vBuffer.v[indices.z]);
    vec3[3] localPositions = vec3[3](vertices[0].position, vertices[1].position, vertices[2].position);
    vec3[3] localNormals = vec3[3](vertices[0].normal, vertices[1].normal, vertices[2].normal);

    const vec3 barycentrics = vec3(1.0 - hitAttribs.x - hitAttribs.y, hitAttribs.x, hitAttribs.y);

    HitPoint hitPoint;
    hitPoint.position = getWorldPosition(localPositions, barycentrics);
    hitPoint.normal = getWorldNormal(localNormals, barycentrics);

    getCoordinateFrame_Duff(hitPoint.normal, hitPoint.tangent, hitPoint.bitangent);
    return hitPoint;
}