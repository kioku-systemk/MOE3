VertexShader = [[attribute vec3 pos;attribute vec4 col;attribute vec3 nor;attribute vec2 uv;uniform sampler2D color_tex;uniform mat4 proj;uniform mat4 view;uniform mat4 world;uniform vec4 p1;varying HIGHP vec2 vtx_uv;varying LOWP vec4    vtx_col;void main(void){	vtx_uv = uv;	vtx_col = col;	gl_Position = proj * view * world * vec4(pos,1);}]]FragmentShader = [[uniform sampler2D color_tex; uniform vec4 color;uniform vec4 time;uniform vec4 p1;uniform vec4 fft;varying HIGHP vec2 vtx_uv;varying LOWP vec4  vtx_col;float PI = 3.1415926;void main(void){	float d = sin(fract((vtx_uv.y*vtx_uv.x*fft.y+time.x)*1.0)*PI);	d = pow(d*1.5,5.0);	gl_FragColor = vec4(d,d,d,1.0) * color * texture2D(color_tex, vtx_uv);}]]GeometryShader = ""DepthTest = trueBlend = trueBlendSrc = "SRC_ALPAH"BlendDst = "ONE_MINUS_SRC_ALPHA"Priority=0