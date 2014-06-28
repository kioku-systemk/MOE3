VertexShader = [[attribute vec3 pos;attribute vec4 col;attribute vec3 nor;attribute vec2 uv;uniform sampler2D color_tex;uniform mat4 proj;uniform mat4 view;uniform mat4 world;uniform vec4 p1;varying HIGHP vec2 vtx_uv;varying LOWP vec4    vtx_col;varying HIGHP vec3 vtx_nor;varying HIGHP vec4 vtx_pos;void main(void){	vtx_uv = uv;	vtx_col = col;	vtx_nor = vec4(world * vec4(nor,0)).xyz;	vtx_pos = world * vec4(pos, 1);	gl_Position = proj * view * vtx_pos;// world * vec4(pos, 1);}]]FragmentShader = [[uniform sampler2D color_tex; uniform vec4 color;uniform vec4 time;uniform vec4 p1;uniform vec4 eyepos;varying HIGHP vec3 vtx_nor;varying HIGHP vec2 vtx_uv;varying LOWP vec4  vtx_col;varying HIGHP vec4 vtx_pos;void main(void){	vec3 eyedir = normalize(vtx_pos.xyz - eyepos.xyz);	vec3 ref = reflect(vtx_nor, eyedir);	float s1 = pow(max(0.0, dot(ref, normalize(vec3(-0.5, 0.5, 1)))), 2.0);	float s2 = pow(max(0.0, dot(ref, normalize(vec3(1.0, 0.5, -1)))), 2.0);	float s3 = pow(max(0.0, dot(ref, normalize(vec3(-0.5, -0.5, -0.5)))), 2.0);	vec3 s = s1*vec3(1,0.9,0.9) + s2*vec3(0.9,0.95,0.9)+s3*vec3(0.9,0.9,1);	vec3 spec = s*1.5;//vec3(s*2.0, s*2.0, s*2.0);	float d = dot(vec3(normalize(vtx_nor)), vec3(0,1,0)) * 0.5 + 0.5;	vec4 dcol = sqrt(vec4(d) * color);	gl_FragColor = vec4(spec,1.0) * texture2D(color_tex, vtx_uv);}]]GeometryShader = ""DepthTest = trueBlend = trueBlendSrc = "SRC_ALPAH"BlendDst = "ONE_MINUS_SRC_ALPHA"Priority=1