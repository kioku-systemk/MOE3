VertexShader = [[attribute vec3 pos;attribute vec4 col;attribute vec3 nor;attribute vec2 uv;uniform mat4 proj;uniform mat4 view;uniform mat4 world;uniform vec4 p1;varying HIGHP vec2 vtx_uv;varying LOWP vec4  vtx_col;varying HIGHP vec3 vtx_nor;varying HIGHP vec4 vtx_pos;void main(){vec3 N = nor;//N.y = 0;	vtx_nor = normalize((world * vec4(N,0)).xyz);	;	vtx_uv = uv;	vtx_pos = vec4(pos,1);	vtx_col = col;	vec4 planepos = world * vec4(pos,1.0);	float shs = 0.05;	vec3 ns = vec3(vtx_nor.x*shs*planepos.y, vtx_nor.y*0.05, vtx_nor.z*shs*planepos.y); 	planepos += vec4(ns,0.0);	float h = 1.0 - planepos.y*0.5;	vtx_col = vec4(1,1,1,h);	float groundpos = 0.35;	planepos.y = groundpos + 0.001*vtx_nor.z+ 0.001*vtx_nor.x;    gl_Position = proj * view * planepos;}]]FragmentShader = [[uniform sampler2D color_tex; uniform vec4 color;uniform vec4 time;uniform vec4 p1;uniform vec4 eyepos;varying HIGHP vec3 vtx_nor;varying HIGHP vec2 vtx_uv;varying LOWP vec4  vtx_col;varying HIGHP vec4 vtx_pos;void main(){	if (color.x + color.y + color.z >2.5)		discard;	vec3 n = normalize(vtx_nor);	n.y = 0;		float d = length(n); 	gl_FragColor = vec4(0,0,0,(1.0-d)*0.5*vtx_col.a);	return;}]]GeometryShader = ""DepthTest = trueBlend = trueBlendSrc = "SRC_ALPAH"BlendDst = "ONE_MINUS_SRC_ALPHA"Priority = 1