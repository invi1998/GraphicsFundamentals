#define TMIN 0.1
#define TMAX 20.
#define RAYMARCH_TIME 128   // 最大迭代次数
#define PRECISION   0.001   // 定义精度
#define AA 3    

vec2 fixUV(in vec2 c) {
    return (2. * c - iResolution.xy) / min(iResolution.x, iResolution.y);
}

// 定义3维SDF球体
float sdfSphere(in vec3 p) {
    // 空间中的点到球体的距离就是坐标的长度减去球体的半径
    return length(p) - 0.3;
}

// 射线源头，射线方向
float rayMarch(in vec3 ro, in vec3 rd) {
    // 走的距离t
    float t = TMIN;
    for (int i = 0; i < RAYMARCH_TIME && t < TMAX; i++) {
        vec3 p = ro + t * rd;
        float d = sdfSphere(p);
        if (d < PRECISION) {
            break;
        }
        t += d;
    }
    return t;
}

vec3 calcNormal(in vec3 p) {
    const float h = 0.0001;
    const vec2 k = vec2(1, -1);
    return normalize(
        k.xyy * sdfSphere(p + k.xyy*h) +
        k.yyx * sdfSphere(p + k.yyx*h) +
        k.yxy * sdfSphere(p + k.yxy*h) +
        k.xxx * sdfSphere(p + k.xxx*h)
    );
}

vec3 render(vec2 uv) {
    vec3 color = vec3(0.);
    // 定义摄像机
    vec3 ro = vec3(0., 0., -2.);
    // 定义射线方向（从摄像机到屏幕上任意点的方向）,然后归一化（单位向量）
    vec3 rd = normalize(vec3(uv, 0.) - ro);

    float t = rayMarch(ro, rd);

    if (t < TMAX) {
        // 焦点坐标（在这个例子中就是球体表面坐标
        vec3 p = ro + t*rd;
        // 法向量
        vec3 n = calcNormal(p);
        // 定义光线源
        vec3 light = vec3(cos(iTime), 3., sin(iTime) - 2.);
        // 计算光线和法向量的夹角余弦（做点乘）
        float dif = clamp(dot(normalize(light - p) , n), 0., 1.);
        // 添加环境光
        float amp = 0.2 + 0.3 * dot(n, vec3(0., 0., -1.));
        color = amp * vec3(1, 0.2, 0.5) + dif * vec3(0.8);
    }

    // 伽马校正（对0-1取一个n次幂，以防环境光太低导致模型暗部过暗），这里就取1/2次幂
    return sqrt(color);
}

void mainImage(out vec4 fragColor, in vec2 fragCoord) {
    vec3 color = vec3(0.);
    for(int m = 0; m < AA; m++){
        for (int n = 0; n < AA; n++){
            vec2 offset = 2. * (vec2(float(m), float(n)) / float(AA) - 0.5);
            vec2 uv = fixUV(fragCoord + offset);
            color += render(uv);

        }
    }

    fragColor = vec4(color / float(AA * AA), 1.);
}

// 3维的SDF绘制比2维SDF要难很多，因为涉及到很多立体操作，光线，材质等