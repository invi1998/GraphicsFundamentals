#iChannel1 "file://girls1.png"
#iChannel2 "file://girls2.png"
#iChannel3 "file://girls4.png"
#iChannel4 "file://girls3.png"
#iChannel5 "file://girls5.png"
#iChannel6 "file://girls6.png"

#iChannel1::WrapMode "Repeat"
#iChannel2::WrapMode "Repeat"
#iChannel3::WrapMode "Repeat"
#iChannel4::WrapMode "Repeat"
#iChannel5::WrapMode "Repeat"
#iChannel6::WrapMode "Repeat"


#define AA 3
#define PRECISION .0001
#define MAX_DIST 20.
#define MIN_DIST .01
#define MAX_ITER 128
#define PI 3.14159265

vec2 fixUV(in vec2 uv) {
    return (2. * uv - iResolution.xy) / iResolution.y;
}

float sdfSphere(in vec3 p, in float r) {
    return length(p) - r;
}

float sdfPlane(in vec3 p) {
    return p.y;
}

float sdfBox(in vec3 p, in vec3 b, float rad) {
    vec3 d = abs(p) - b;
    return length(max(d, 0.)) + min(max(d.x, max(d.y, d.z)), 0.) - rad;
}

// https://iquilezles.org/articles/smin
float smin(float a, float b, float k) {
    float h = clamp(0.5 + 0.5 * (b - a) / k, 0.0, 1.0);
    return mix(b, a, h) - k * h * (1.0 - h);
}

float map(in vec3 p) {
    // 球体
    // float d = sdfSphere(p, 0.7);

    float d = sdfBox(p - vec3(0., 1., 0.), vec3(0.5), 0.);
    // d = smin(d, sdfPlane(p), 1. + 0.5 * sin(0.57 * iTime));
    
    return d;
}

// https://iquilezles.org/articles/normalsSDF
vec3 calcNormal(in vec3 p) {
    const float h = 0.0001;
    const vec2 k = vec2(1, -1);
    return normalize(k.xyy * map(p + k.xyy * h) +
        k.yyx * map(p + k.yyx * h) +
        k.yxy * map(p + k.yxy * h) +
        k.xxx * map(p + k.xxx * h));
}

float rayMarch(in vec3 ro, in vec3 rd) {
    float t = MIN_DIST;
    for(int i = 0; i < MAX_ITER && t < MAX_DIST; i++) {
        vec3 p = ro + t * rd;
        float d = map(p);
        if(d < PRECISION) {
            break;
        }
        t += d;
    }
    return t;
}

// https://iquilezles.org/articles/rmshadows
float softshadow(in vec3 ro, in vec3 rd, float k) {
    float res = 1.0;
    float ph = 1e20;
    for(float t = MIN_DIST; t < MAX_DIST;) {
        float h = map(ro + rd * t);
        if(h < PRECISION)
            return 0.0;
        float y = h * h / (2.0 * ph);
        float d = sqrt(h * h - y * y);
        res = min(res, k * d / max(0.0, t - y));
        ph = h;
        t += h;
    }
    return res;
}

mat3 setCamera(vec3 ta, vec3 ro, float cr) {
    vec3 z = normalize(ta - ro);
    vec3 cp = vec3(sin(cr), cos(cr), 0.);
    vec3 x = normalize(cross(z, cp));
    vec3 y = cross(x, z);
    return mat3(x, y, z);
}

// https://iquilezles.org/articles/nvscene2008/rwwtt.pdf
float calcAO(vec3 p, vec3 n) {
    // 计算环境光遮蔽
    float occ = 0.0;
    float sca = 1.0;
    for(float i = 0.; i < 5.; i += 1.) {
        float h = 0.01 + 0.03 * i;
        float d = map(p + n * h);
        occ += (h - d) * sca;
        sca *= 0.95;
        if(occ > 0.35)
            break;
    }
    return clamp(1. - 3. * occ, 0., 1.) * (.5 + .5 * n.y);
}

// 如果是只考虑平面对物体的环境光遮蔽，可以采用下面这种简单的计算方式
float calcAO2(vec3 n) {
    return 0.5 + 0.5 * n.y;
}

vec3 render(vec2 uv) {
    vec3 color = vec3(0.);
    vec3 ro = vec3(3. * cos(iTime), 2.5, 3. * sin(iTime));
    // vec3 ro = vec3(1.5, 1.5, 1.);
    if (iMouse.z > 0.01) {
        float theta = iMouse.x / iResolution.x * 3. * PI;
        ro = vec3(3. * cos(theta), 3. * (-3. * iMouse.y / iResolution.y + 1.), 3. * sin(theta));
    }
    vec3 ta = vec3(0.);
    mat3 cam = setCamera(ta, ro, 0.);
    vec3 rd = normalize(cam * vec3(uv, 1.));
    float t = rayMarch(ro, rd);
    if(t < MAX_DIST) {
        vec3 p = ro + t * rd;
        vec3 n = calcNormal(p);
        vec3 light = vec3(2., 1., 0.);
        float dif = clamp(dot(normalize(light - p), n), 0., 1.);
        float amb = 0.5 + 0.5 * dot(n, vec3(0., 1., 0.));

        // color = texture(iChannel1, p.xy).rgb;
        // color *= n.z;
        // color += texture(iChannel2, p.xy).rgb;
        // color *= n.z;

        // color = amb * vec3(.7) + dif * vec3(1.);
        vec3 colorXY = texture(iChannel1, p.xy + 0.5).rgb;
        vec3 colorYX = texture(iChannel2, p.xy + 0.5).rgb;
        vec3 colorXZ = texture(iChannel3, p.zx + 0.5).rgb;
        vec3 colorZX = texture(iChannel4, p.xz + 0.5).rgb;
        vec3 colorYZ = texture(iChannel5, p.zy + 0.5).rgb;
        vec3 colorZY = texture(iChannel6, p.zy + 0.5).rgb;
        if (n.z > 0.) {
            color = colorXY * n.z;
        } else if (n.z < 0.) {
            color = colorYX * -n.z;
        }
        if (n.y > 0.) {
            color += colorXZ * n.y;
        } else if (n.y < 0.) {
            color += colorZX * -n.y;
        }
        if (n.x > 0.) {
            color += colorYZ * n.x;
        } else if (n.x < 0.) {
            color += colorZY * -n.x;
        }
        //  + colorYX * -n.z;
        
        // 球体贴图
        // n = abs(n);
        // n = pow(n, vec3(10.));
        // // n /= n.x + n.y + n.z;
        // // color = colorXY * n.z + colorXZ * n.y + colorYZ * n.x;
        // color = n;
    }
    return color;
}

void mainImage(out vec4 fragColor, in vec2 fragCoord) {
    vec3 color = vec3(0.);
    for(int m = 0; m < AA; m++) {
        for(int n = 0; n < AA; n++) {
            vec2 offset = 2. * (vec2(float(m), float(n)) / float(AA) - .5);
            vec2 uv = fixUV(fragCoord + offset);
            color += render(uv);
        }
    }
    fragColor = vec4(color / float(AA * AA), 1.);
}
