property sampler2D tex;
property vec3 position = vec3(0.0, 0.0, 0.0);
property int multiplier;
property mat4 transform;

shader vertex {
    position = position * vec3(float(multiplier));
    vec4 position = vec4(position, 1.0);
    vec3 myVec = position.xyz + vec3(position.xx, 0.0);

    require foo {
        position = vec4(0.0);
    }

    float first = position.x;
    int iFirst = int(first);
    int iSecond = int(position[1]);
    bool equal = iFirst == iSecond && bool(first);
    mat4 identity = mat4(1.0);
    mat3 identity3 = mat3(identity);
    vec3 firstCol = identity3[0].xyz;
    vec4 transformed = transform * identity * vec4(firstCol, 1.0);

    return position + vec4(float(equal));
}
