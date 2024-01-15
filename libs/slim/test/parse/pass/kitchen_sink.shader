#colorpicker
property vec3 color = vec3(1.0, 1.0, 1.0);

feature texture {
    #display "Texture"
    property sampler2D tex;
}

shared vec3 xyz;
shared vec2 uv;
shared vec3 normal;
