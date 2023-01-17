#define AA 3
#define PRECISION .0001
#define MAX_DIST 20.
#define MIN_DIST .01
#define MAX_ITER 128

vec2 fixUV(in vec2 uv) {
    return (2. * uv - iResolution.xy) / iResolution.y;
}

float sdfSphere(in vec3 p, in float r) {
    return length(p) - r;
}

float sdfPlane(in vec3 p) {
    return p.y;
}

// https://iquilezles.org/articles/smin
float smin(float a, float b, float k) {
    float h = clamp(0.5 + 0.5 * (b - a) / k, 0.0, 1.0);
    return mix(b, a, h) - k * h * (1.0 - h);
}

float map(in vec3 p) {
    float d = sdfSphere(p - vec3(0., 1., 0.), 1.);
    d = smin(d, sdfPlane(p), 1. + 0.5 * sin(0.57 * iTime));
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

vec3 render(in vec2 uv) {
    vec3 color = vec3(0.);

    // set camera
    vec3 target = vec3(0.);
    float camHigh = 2.;
    float camRad = 2.5;
    vec3 camLoc = vec3(camRad * cos(0.2 * iTime), camHigh, camRad * sin(0.2 * iTime));
    mat3 camMat = setCamera(target, camLoc, 0.);

    // calc rayMarching
    vec3 rd = normalize(camMat * vec3(uv, 1.));
    float t = rayMarch(camLoc, rd);

    // set light
    vec3 lightLoc = vec3(3., 5., 0.);
    vec3 lightColor = vec3(1.);
    vec3 ambColor = vec3(0.23);

    if(t < MAX_DIST) {
        // calc light
        vec3 p = camLoc + rd * t;
        vec3 n = calcNormal(p);
        vec3 ambDirect = vec3(0., 1., 0.);
        float dif = clamp(dot(normalize(lightLoc - p), n), 0., 1.);
        float amb = 0.5 + 0.5 * dot(n, normalize(ambDirect));

        // calc shadow
        float shadow = softshadow(p, normalize(lightLoc - p), 7.);
        color = amb * ambColor + dif * lightColor * shadow;

        // calc AO
        float ao = calcAO(p, n);
        ao *= calcAO2(n);
        color *= ao;
    }

    return sqrt(color);
}

void mainImage(out vec4 fragColor, in vec2 fragCoord) {
    vec3 col = vec3(0.);

    for(int x = 0; x < AA; x++) {
        for(int y = 0; y < AA; y++) {
            vec2 offset = vec2(float(x) / float(AA) - .5, float(y) / float(AA) - .5);
            col += render(fixUV(fragCoord + offset));
        }
    }
    col /= float(AA * AA);

    fragColor = vec4(col, 1.0);
}