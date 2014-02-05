VertexShader = [[attribute vec3 pos;attribute vec4 col;attribute vec3 nor;attribute vec2 uv;uniform sampler2D color_tex;uniform mat4 proj;uniform mat4 view;uniform mat4 world;uniform vec4 p1;varying HIGHP vec2 vtx_uv;varying LOWP vec4    vtx_col;void main(void){	vtx_uv = uv;	vtx_col = col;	gl_Position = proj * view * world * vec4(pos+vec3(0.0,-0.2,0.8),1);}]]FragmentShader = [[uniform sampler2D color_tex; uniform vec4 color;uniform vec4 time;uniform vec4 demotime;uniform vec4 p1;varying HIGHP vec2 vtx_uv;varying LOWP vec4  vtx_col;float PI = 3.1415926;float random(vec2 p){  const vec2 r = vec2(    23.1406926327792690,  // e^pi (Gelfond's constant)     2.6651441426902251); // 2^sqrt(2) (Gelfond–Schneider constant)  return fract( cos( mod(123456789., 1e-7 + 256. * dot(p,r) ) ) );  }void main(void){	float lv = 2.5;	vec2 uv = vtx_uv;	uv.x = 0.0;	uv.y = floor((uv.y+fract(time.x))*100.0);///vec2(10.0);	float tr = random(uv);	/*float ta = max(0.0,sin((time.x+2.5)*10.0*PI+3.0));	if (demotime.x < 36.015)*/	float ta = max(0.0,sin(time.x*8.0*PI+3.0));	if (demotime.x < 33.715)		ta = 0.0;	vec4 cl = vec4(tr,tr,tr,ta);//vec4(1.0, 0.19, 0.45, 1.0);	vec4 pnk = vec4(1.0, 0.19, 0.45, 1.0);	gl_FragColor = pnk * lv * cl * texture2D(color_tex, vtx_uv);	if (gl_FragColor.a < 0.05)		discard;}]]GeometryShader = ""DepthTest = trueBlend = trueBlendSrc = "SRC_ALPAH"BlendDst = "ONE"--_MINUS_SRC_ALPHA"Priority=10