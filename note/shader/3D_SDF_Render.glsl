#define TMIN 0.1
#define TMAX 20.
#define RAYMARCH_TIME 128   // 最大迭代次数
#define PRECISION   0.001   // 定义精度
#define AA 3
#define PI 3.1415936

vec2 fixUV(in vec2 c) {
    return (2. * c - iResolution.xy) / min(iResolution.x, iResolution.y);
}

// 定义平面
float sdfPlane(in vec3 p) {
    return p.y;
}
// 定义3维SDF球体
float sdfSphere(in vec3 p) {
    // 空间中的点到球体的距离就是坐标的长度减去球体的半径
    return length(p) - 1.;
}

// 3维立方体
float sdfRect(in vec3 p, in vec3 b) {
    vec3 d = abs(p) - b;
    return length(max(d, 0.)) + min(max(d.x, max(d.y, d.z)), 0.);
}

float map(in vec3 p) {
    float d = sdfSphere(p);
    d = min(d, sdfPlane(p + vec3(0., 1., 0.)));
    return d;
}

// 射线源头，射线方向
float rayMarch(in vec3 ro, in vec3 rd) {
    // 走的距离t
    float t = TMIN;
    for (int i = 0; i < RAYMARCH_TIME && t < TMAX; i++) {
        vec3 p = ro + t * rd;
        // float d = sdfSphere(p);
        float d = map(p);
        // float d = sdfRect(p, vec3(.4, .4, .5));
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
        k.xyy * map(p + k.xyy*h) +
        k.yyx * map(p + k.yyx*h) +
        k.yxy * map(p + k.yxy*h) +
        k.xxx * map(p + k.xxx*h)
    );
}

// 设置相机（target, 摄像机位置，还有θ角）
mat3 setCamera(vec3 ta, vec3 ro, float cr) {
    // 确定z轴
    vec3 z = normalize(ta - ro);
    vec3 cp = vec3(sin(cr), cos(cr), 0.);
    // x轴 z叉乘cp，然后正则化（归一化，单位向量）
    vec3 x = normalize(cross(z, cp));
    // y轴 x叉乘z
    vec3 y = cross(x, z);

    return mat3(x, y, z);
}

// 软阴影
float softShadow(in vec3 ro, in vec3 rd, float k) {
    // float res = 1.0;
    // for (float t = TMIN; t < TMAX;) {
    //     float h = map(ro + rd*t);
    //     if (h < 0.001) {
    //         return 0.0;
    //     }
    //     res = min(res, k*h/t);
    //     t += h;
    // }
    // return res;

    // 改良版本
    float res = 1.0;
    float ph = 1e20;
    for (float t = TMIN; t < TMAX;) {
        float h = map(ro + rd * t);
        if (h < 0.001) {
            return 0.0;
        }
        float y = h * h / (2.0 * ph);
        float d = sqrt(h * h - y * y);
        res = min(res, k * d / max(0.0, t - y));
        ph = h;
        t += h;
    }
    return res;
}

vec3 render(vec2 uv) {
    vec3 color = vec3(0.);
    // 定义摄像机
    vec3 ro = vec3(0., 1., -2.);
    // vec3 ro = vec3(2. * cos(iTime), 1., 2. * sin(iTime));

    if (iMouse.z > 0.01) {
        float theta = iMouse.x / iResolution.x * 2. * PI;
        float thetay = iMouse.y / iResolution.y *2. *PI;
        ro = vec3(2. * cos(theta), 2. * sin(thetay), 2. * sin(theta));
    }

    // 目标方向，朝向中心即可
    vec3 ta = vec3(0.);
    mat3 cam = setCamera(ta, ro, 0.);

    // 定义射线方向（从摄像机到屏幕上任意点的方向）,然后归一化（单位向量）
    vec3 rd = normalize(cam * vec3(uv, 1.));

    float t = rayMarch(ro, rd);

    if (t < TMAX) {
        // 焦点坐标（在这个例子中就是球体表面坐标
        vec3 p = ro + t*rd;
        // 法向量
        vec3 n = calcNormal(p);
        // 定义光线源
        vec3 light = vec3(cos(iTime), 2., sin(iTime) - 2.);
        // vec3 light = vec3(2., 3., 0.);
        // 计算光线和法向量的夹角余弦（做点乘）
        float dif = clamp(dot(normalize(light - p), n), 0.1, 1.);

        // 添加阴影
        p += PRECISION * n;
        float st = softShadow(p, normalize(light - p), 10.);
        dif *= st;

        // 添加环境光
        float amp = 0.5 + 0.3 * dot(n, vec3(0., 0., -1.));
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