VertexShader = [[attribute vec3 pos;attribute vec4 col;attribute vec3 nor;attribute vec2 uv;uniform sampler2D color_tex;uniform mat4 proj;uniform mat4 view;uniform mat4 world;uniform vec4 p1;varying HIGHP vec2 vtx_uv;varying LOWP vec4    vtx_col;varying HIGHP vec4 vtx_pos;void main(void){	vtx_uv = uv;	vtx_col = col;	vtx_pos = world * vec4(pos,1);	gl_Position = proj * view * vtx_pos;}]]FragmentShader = [[uniform sampler2D color_tex; uniform vec4 color;uniform vec4 time;uniform vec4 p1;varying HIGHP vec2 vtx_uv;varying LOWP vec4  vtx_col;varying HIGHP vec4 vtx_pos;float PI = 3.1415926;void main(void){	float d = 0.5 + 0.5*sin(time.x*20.0+vtx_pos.y*30.0);	gl_FragColor = vec4(3.0*d,2.0*d,4.0*d,1.0) * texture2D(color_tex, vtx_uv);	if (gl_FragColor.a < 0.05)		discard;}]]GeometryShader = ""DepthTest = trueBlend = trueBlendSrc = "SRC_ALPAH"BlendDst = "ONE_MINUS_SRC_ALPHA"Priority=11