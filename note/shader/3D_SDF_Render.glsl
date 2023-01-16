#define TMIN 0.1
#define TMAX 40.
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
float sdfBox(in vec3 p, in vec3 b, float rad) {
    vec3 d = abs(p) - b;
    return length(max(d, 0.)) + min(max(d.x, max(d.y, d.z)), 0.) - rad;
}

vec2 opU(vec2 a, vec2 b) {
    return a.x < b.x ? a : b; 
}

// 这里返回一个二维向量，第一个值还是他的最短距离，第二个用于标记这个物体（现在要做物体区分）
vec2 map(in vec3 p) {
    // float d = sdfSphere(p);
    // // float d = sdfBox(p, vec3(0.4, 1., 0.3));
    // d = min(d, sdfPlane(p + vec3(0., 1., 0.)));
    // return d;

    // d = opU(d, vec2(sdfPlane(p + vec3(0., 1., 0.)), 1.));       // 平面标记为1

    // vec2 d = vec2(sdfSphere(p - vec3(0., 1., 0.)), 2.);    // 球体标记为2
    // d = opU(d, vec2(sdfBox(p - vec3(-2., 1., 1.4), vec3(0.7, 1., 1.)), 3.));   // 长方体标记为3

    vec2 d = vec2(sdfBox(p - vec3(0., 1., 0.), vec3(1.5, 0.8, 1.), 0.3), 2);

    return d;
}

// 射线源头，射线方向
vec2 rayMarch(in vec3 ro, in vec3 rd) {
    // 走的距离t
    float t = TMIN;
    float tmax = TMAX;
    vec2 res = vec2(-1.);

    if (rd.y < 0.) {
        float tp = -ro.y / rd.y;
        tmax = min(tp, tmax);
        res = vec2(tp, 1.);
    }

    for (int i = 0; i < RAYMARCH_TIME && t < TMAX; i++) {
        vec3 p = ro + t * rd;
        // float d = sdfSphere(p);
        vec2 d = map(p);
        // float d = sdfBox(p, vec3(.4, .4, .5));
        if (d.x < PRECISION) {
            res = vec2(t, d.y);
            break;
        }
        t += d.x;
    }
    return res;
}

vec3 calcNormal(in vec3 p) {
    const float h = 0.0001;
    const vec2 k = vec2(1, -1);
    return normalize(
        k.xyy * map(p + k.xyy*h).x +
        k.yyx * map(p + k.yyx*h).x +
        k.yxy * map(p + k.yxy*h).x +
        k.xxx * map(p + k.xxx*h).x
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
    float tmin = 0.1;
    float tmax = 10.;
    for (float t = tmin; t < tmax;) {
        float h = map(ro + rd * t).x;
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

vec2 tri(in vec2 x) {
    vec2 h = fract(x*0.5) - 0.5;
    return 1. - 2. * abs(h);
}

float checkersGrid(in vec2 uv, in vec2 ddx, in vec2 ddy) {
    vec2 w = max(abs(ddx), abs(ddy)) + 0.01;
    vec2 i = (tri(uv + 0.5 * w) - tri(uv-0.5*w)) / w;
    return 0.5 - 0.5*i.x * i.y;
}

vec3 render(vec2 uv, in vec2 px, in vec2 py) {
    // 定义摄像机
    // vec3 ro = vec3(0., 3., -6.);
    vec3 ro = vec3(4. * cos(.1 * iTime), 3., 6. * sin(.1 * iTime));

    if (iMouse.z > 0.01) {
        float theta = iMouse.x / iResolution.x * 2. * PI;
        float thetay = iMouse.y / iResolution.y *2. *PI;
        ro = vec3(4. * cos(theta), 3. * sin(thetay), 4. * sin(theta));
    }

    // 目标方向，朝向中心即可
    vec3 ta = vec3(0., 1., 0);
    mat3 cam = setCamera(ta, ro, 0.);
    // 定义焦距
    float fl = 1.;

    // 定义射线方向（从摄像机到屏幕上任意点的方向）,然后归一化（单位向量）
    vec3 rd = normalize(cam * vec3(uv, 1.));

    vec3 bg = vec3(.7, .7, .9);
    vec3 color = bg - rd.y;

    vec2 t = rayMarch(ro, rd);

    if (t.y > 0.) {
        // 焦点坐标（在这个例子中就是球体表面坐标
        vec3 p = ro + t.x*rd;
        // 法向量
        vec3 n = (t.y < 1.1) ? vec3(0, 1, 0) : calcNormal(p);
        // 定义光线源
        // vec3 light = vec3(2. * cos(iTime), 6., sin(iTime) - 2.);
        vec3 light = vec3(2., 7., -2.);
        // 计算光线和法向量的夹角余弦（做点乘）
        float dif = clamp(dot(normalize(light - p), n), 0.1, 1.);

        // 添加阴影
        p += PRECISION * n;
        dif *= softShadow(p, normalize(light - p), 10.);

        // 添加环境光
        float amp = 0.9 + 0.3 * dot(n, vec3(0., 0., -1.));

        vec3 c = vec3(0.);

        if (t.y > 2.9 && t.y < 3.1) {
            // 长方体
            c = vec3(1., 0., 0.2);
        } else if (t.y > 1.9 && t.y < 2.1) {
            // 球体 - > 身体
            c = vec3(0.2, 0.8, 0.93);
        } else if (t.y > 0.9 && t.y < 1.1) {
            // 平面
            // vec2 grid = floor(p.xz);
            // c = vec3(0.) + 1. * mod(grid.x + grid.y, 2.);
            vec3 rdx = normalize(cam * vec3(px, fl));
            vec3 rdy = normalize(cam * vec3(py, fl));

            vec3 ddx = ro.y * (rd / rd.y - rdx/rdx.y);
            vec3 ddy = ro.y * (rd / rd.y - rdy/rdy.y);

            c = vec3(0.) + vec3(0.9) * checkersGrid(p.xz, ddx.xz, ddy.xz);
        }

        // color = amp * vec3(1, 0.2, 0.5) + dif * vec3(0.8);
        color = amp * c + dif * vec3(0.5);
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
            vec2 px = fixUV(fragCoord + vec2(1., 0.) + offset);
            vec2 py = fixUV(fragCoord + vec2(0., 1.) + offset);
            color += render(uv, px, py);

        }
    }

    fragColor = vec4(color / float(AA * AA), 1.);
}

// 3维的SDF绘制比2维SDF要难很多，因为涉及到很多立体操作，光线，材质等