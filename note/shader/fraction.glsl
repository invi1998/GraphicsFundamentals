// 分形

vec2 fixUV(in vec2 c) {
    return 2. * (2. * c - iResolution.xy) / min(iResolution.x, iResolution.y);
}

// 定义复数的除法
vec2 cdiv(vec2 a, vec2 b) {
    float d = dot(b, b);
    return vec2(dot(a, a), a.y * b.x - a.x * b.y) / d;
}

// 复数的幂运算
vec2 cpow(vec2 a, float n) {
    float l = length(a);
    float an = atan(a.y, a.x);
    return vec2(cos(an*n), sin(an*n)) * pow(l, n);
}

// 朱丽叶集合
vec2 cuml(in vec2 a, in vec2 b) {
    return vec2(a.x * b.x - a.y * b.y, a.y * b.x + a.x * b.y);
}

vec2 integral(vec2 x) {
    x /= 2.;
    return floor(x) + max(2. * fract(x) - 1., 0.);
    // 该积分函数的效果就是0-1之间是平的，然后1-2之间是斜向上的，斜率为1,2-3之间是平的，3-4之间是斜的，斜率为1.。。。
}

float checkerBoard(vec2 p) {
    vec2 fw = fwidth(p);
    vec2 i = integral(p + 0.5 * fw) - integral(p-0.5*fw);
    i /= fw;
    return i.x + i.y - 2. * i.x * i.y;
}

float grid(vec2 p) {
    vec2 q = p * 16.;
    return 0.5 + 0.5 * checkerBoard(q);
}

void mainImage(out vec4 fragColor, in vec2 fragCoord) {
    vec2 uv = fixUV(fragCoord);
    vec3 col = vec3(1.0);
    
    vec2 z = uv;
    vec2 c = vec2(0.28, -0.49);

    // 分形迭代次数
    float t = 1.;

    vec2 dz = vec2(1., 0);
    vec2 phi = z;

    for (;t<512.; t++) {
        if (length(z) > 1024.) {
            break;
        }
        dz = cuml(z, dz) * 2.;
        z = cuml(z, z) + c;

        float n = pow(0.5, t);
        vec2 c = cdiv(z, z-c);
        phi = cuml(phi, cpow(c, n)); 
    }

    if (t > 511.) {
        col = vec3(0.53, 0.21, 0.35);
    } else {
        float d = length(z) * log(length(z)) / length(dz);
        col *= smoothstep(0., 0.01, d);
        col *= grid(phi);
        // 查看距离场
        // col *= 0.7 + 0.2 * sin(120. * d);
    }


    fragColor = vec4(col, 1);
}