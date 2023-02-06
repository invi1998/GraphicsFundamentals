vec2 fixUV(in vec2 c){
    return 2. * (2.*c - iResolution.xy) / min(iResolution.x, iResolution.y);
}

float Circle(in vec2 p, in vec2 c){
    float d = length(p-c);
    return smoothstep(fwidth(d), 0., d-0.02);
}

float Line(in vec2 p, in vec2 a, in vec2 b) {
    vec2 pa = p-a, ba = b-a;
    float t = clamp(dot(pa, ba)/dot(ba,ba), 0., 1.);
    vec2 c = a+ba*t;
    float d = length(c-p);
    return smoothstep(fwidth(d), 0., d-0.001);
}

vec2 Bizier(in vec2 A, vec2 B, vec2 C, float t) {
    vec3 col = vec3(0.);
    vec2 AC = mix(A, C, t);
    vec2 CB = mix(C, B, t);
    vec2 Bi = mix(AC, CB, t);

    return Bi;
}

void mainImage(out vec4 fragColor, in vec2 fragCoord) {
    vec2 uv = fixUV(fragCoord);

    vec3 col = vec3(.1, .1, .1);

    float t = sin(iTime) * 0.5 + 0.5;

    // 贝塞尔起始点，结束点，控制点（用鼠标控制），AC线性差值t点，CB线性差值t点, 
    vec2 A = 
        vec2(-0.9, -0.9),
        B = vec2(0.7, 0.8),
        C = fixUV(iMouse.xy),
        AC = mix(A, C, t),
        CB = mix(C, B, t),
        Bi = mix(AC, CB, t);
    col += vec3(0.8, 0.2, 0.4)*Circle(uv, A);
    col += vec3(0.3, 0.1, 0.8)*Circle(uv, B);
    col += vec3(0.3, 0.9, 0.1)*Circle(uv, C);
    col += vec3(0.9, 0.4, 0.1)*Circle(uv, AC);
    col += vec3(0.1, 0.5, 0.8)*Circle(uv, CB);
    col += vec3(0.9, 0.1, 0.)*Circle(uv, Bi);

    col += vec3(0.1) * Line(uv, A, C);
    col += vec3(0.1) * Line(uv, C, B);
    col += vec3(0.1) * Line(uv, AC, CB);

    int NUM_SEGS = 50;

    vec2 p, pp = A;
    for (int i=1; i<NUM_SEGS;i++){
        t = float(i)/float(NUM_SEGS);
        p = Bizier(A, B, C, t);
        col += vec3(0.9, 0.2, t)*Line(uv, p, pp);
        pp = p;
    }

    fragColor = vec4(col, 0.3);
}
