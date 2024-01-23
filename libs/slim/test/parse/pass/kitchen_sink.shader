#colorpicker
property vec3 color = vec3(1.0, 1.0, 1.0);
property vec3 sunDir = vec3(0.0, -1.0, 0.0);

feature texture {
    #display "Texture"
    property sampler2D tex;
}

shared vec3 xyz;
shared vec2 uv;
shared vec3 normal;

shader vertex {
    normal = LOCAL_TO_WORLD_MX * vec4(V_NORMAL, 0.0);
    xyz = LOCAL_TO_WORLD_MX * vec4(V_NORMAL, 1.0);
    uv = V_UV;
    return LOCAL_TO_CLIP_MX * vec4(V_XYZ, 1.0);
}

shader fragment {
    color = color * texture(tex, uv).rgb;
    color = color * dot(-sunDir, vec3(normal));
    return color;
}
