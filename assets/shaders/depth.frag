precision lowp float;

varying float Depth;


void main()
{
    gl_FragColor = vec4(vec3(Depth), 1.0);
}

