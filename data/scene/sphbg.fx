VertexShader = [[attribute vec3 pos;attribute vec4 col;attribute vec3 nor;attribute vec2 uv;uniform sampler2D color_tex;uniform mat4 proj;uniform mat4 view;uniform mat4 world;uniform vec4 p1;varying HIGHP vec2 vtx_uv;varying HIGHP vec3 vtx_nor;varying LOWP vec4    vtx_col;void main(void){	vtx_uv = uv;	vtx_col = col;	vtx_nor = normalize(nor);	gl_Position = proj * view * world * vec4(pos,1);}]]FragmentShader = [[uniform sampler2D color_tex; uniform vec4 color;uniform vec4 time;uniform vec4 p1;varying HIGHP vec2 vtx_uv;varying HIGHP vec3 vtx_nor;varying LOWP vec4  vtx_col;float PI = 3.1415926;void main(void){	vec3 N = normalize(vtx_nor);	float bx = max(0.0,1.0 - time.x) + N.y*0.5+0.8;	gl_FragColor = vec4(bx,bx,bx,1.0);//*color * texture2D(color_tex, vtx_uv);}]]GeometryShader = ""DepthTest = falseBlend = trueBlendSrc = "SRC_ALPAH"BlendDst = "ONE_MINUS_SRC_ALPHA"Priority=1