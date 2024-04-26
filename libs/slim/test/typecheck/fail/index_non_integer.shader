shader vertex {
    vec3 foo = vec3(0.0, 0.0, 0.0);
    foo[1.5];
    return vec4(0.0, 0.0, 0.0, 1.0);
}
