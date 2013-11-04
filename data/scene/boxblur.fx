VertexShader = [[attribute vec3 pos;attribute vec4 col;attribute vec3 nor;attribute vec2 uv;uniform mat4 proj;uniform mat4 view;uniform mat4 world;varying HIGHP vec2 vtx_uv;void main(void){	vtx_uv = pos.xy;	gl_Position = vec4((pos+vec3(-0.5,-0.5,0))*2.0,1.0);}]]FragmentShader = [[uniform sampler2D color_tex; uniform vec4 scolor;uniform sampler2D srcBuf;uniform vec4 size;varying HIGHP vec2 vtx_uv;void main(void){	vec3 ofs[9]; 	ofs[0] = vec3(-1.0/size.x, -1.0/size.y, 0.0625);	ofs[1] = vec3( 1.0/size.x, -1.0/size.y, 0.0625);	ofs[2] = vec3( 0.0/size.x, -1.0/size.y, 0.125);	ofs[3] = vec3(-1.0/size.x,  1.0/size.y, 0.0625);	ofs[4] = vec3( 1.0/size.x,  1.0/size.y, 0.0625);	ofs[5] = vec3( 0.0/size.x,  1.0/size.y, 0.125);	ofs[6] = vec3( 1.0/size.x,  0.0/size.y, 0.125);	ofs[7] = vec3(-1.0/size.x,  0.0/size.y, 0.125);	ofs[8] = vec3( 0.0       ,  0.0       , 0.25);	gl_FragColor = vec4(0);	for (int i = 0; i < 9; ++i)		gl_FragColor += texture2D(srcBuf, vtx_uv+ofs[i].xy) * ofs[i].z;}]]GeometryShader = ""