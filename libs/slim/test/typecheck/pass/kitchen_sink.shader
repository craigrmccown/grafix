property sampler2D tex;
property vec3 position = vec3(0.0, 0.0, 0.0);
property int multiplier;

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

    return position + vec4(float(equal));
}
