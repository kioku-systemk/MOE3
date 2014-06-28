VertexShader = [[attribute vec3 pos;attribute vec4 col;attribute vec3 nor;attribute vec2 uv;uniform mat4 proj;uniform mat4 view;uniform mat4 world;uniform vec4 p1;varying HIGHP vec2 vtx_uv;varying HIGHP vec3 vtx_pos;varying LOWP vec4    vtx_col;void main(void){	vtx_uv = uv;	vtx_col = col;	vtx_pos = pos;	gl_Position = proj * view * world * vec4(pos,1);}]]FragmentShader = [[vec3 lerp(vec3 a, vec3 b, float r) {	return (b - a) * r + a;}uniform sampler2D color_tex; uniform vec4 color;uniform vec4 time;uniform vec4 p1;varying HIGHP vec2 vtx_uv;varying HIGHP vec3 vtx_pos;varying HIGHP vec3    vtx_nor;varying LOWP vec4    vtx_col;void main(void){	float PI = 3.1415926;	vec4 lcol = vtx_col;	float c = max(0.0,pow(sin(2.0*PI*(vtx_uv.y-2.0*time.x-vtx_pos.x+vtx_pos.z)),10.0)*2.0);	c*= (1.5*p1.w);	gl_FragColor = vec4(c,0.0,0.0,1.0);}]]Priority = 5GeometryShader = ""DepthTest = falseBlend = trueBlendSrc = "SRC_ALPHA"BlendDst = "ONE"