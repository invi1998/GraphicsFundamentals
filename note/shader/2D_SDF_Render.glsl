#define R 0.5+0.2*sin(iTime)

// SDF 绘圆。因为sdf放回它记录的是点到图形的最短距离，所以他的返回值是一个float标量
float sdfCircle(in vec2 p) {
    return length(p) - R;
    // 这里p就是一个二维的坐标（注意，不是圆心），r就是半径，这里sdf的返回值是有负数情况的，返回负数就表示在圆内部
}

// SDF绘制矩形
float sdfRect(in vec2 p, vec2 b) {
    vec2 d = abs(p) -b;
    return length(max(d, 0.)) + min(max(d.x, d.y), 0.);
}

vec2 fixUV(in vec2 c){
    return 2. * (2.*c - iResolution.xy) / min(iResolution.x, iResolution.y);
}

void mainImage(out vec4 fragColor, in vec2 fragCoord) {

    vec2 uv = fixUV(fragCoord);

    // float d = sdfCircle(uv);
    
    float d = sdfRect(uv, vec2(1. + 0.2 * sin(iTime), 1. + 0.3 * cos(iTime)));

    vec3 color = 0.1 + sign(d) * vec3(.3, .2, .8);
    // 注意这里：如果在圆里面，因为d是负值，所以sin(d)也就是在[-1,0]这个区间，为负数
    // 所以越靠近圆心，就越黑

    color *= 0.2 - exp(10.*abs(d));
    color *= 0.8 + 0.2*sin(550. * abs(d));

    // 判断鼠标是否按下
    if(iMouse.z > 0.1){
        // 获取当前鼠标的坐标
        vec2 m = fixUV(iMouse.xy);
        // float currentDistance = abs(sdfCircle(m));
        float currentDistance = abs(sdfRect(uv, vec2(1. + 0.2 * sin(iTime), 1. + 0.3 * cos(iTime))));
        color = mix(color, vec3(0.7, 0., 0.6), smoothstep(0.01, 0.001, abs(length(uv - m) - currentDistance)));
        // 绘制鼠标圆心
        color = mix(color, vec3(0.1, 0.8, 0.5), smoothstep(0.01, 0.001, length(uv - m)));
    }

    fragColor = vec4(color, 0.3);
}

// 在一般的图形软件中，如果我们需要定义一个图像（图形），需要将这个图形的各个点，线，面等这些坐标定义出来，
// 但是这种方法在shader里面有点太麻烦了，shader里面有一种更为优雅的绘图方法
// 有向距离场。SDF全称Signed Distance Field，中文一般翻译为有向距离场。
// 距离场，Distance Field，即一种标识了到指定位置的距离参数的数据集合。
// 有向距离场，Signed distance field，则是一种除了记录了距离数值之外，还通过正负号来标识了方向的距离场。正值表示在白色像素外面，负值则表示处于白色像素内部。
// 一个点在空间里面（无论是二维还是三维）任何一个点，到这个形状的最短距离是多少