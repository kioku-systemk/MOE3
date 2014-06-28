VertexShader = [[attribute vec3 pos;attribute vec4 col;attribute vec3 nor;attribute vec2 uv;uniform mat4 proj;uniform mat4 view;uniform mat4 world;varying HIGHP vec2 vtx_uv;varying vec2 coords[14];void main(){    gl_Position = vec4((pos+vec3(-0.5,-0.5,0))*2.0,1.0);    vtx_uv = pos.xy;}]]FragmentShader = [[uniform sampler2D color_tex; uniform vec4 scolor;uniform sampler2D srcBuf;uniform sampler2D srcBuf_depth;uniform vec4 param;varying HIGHP vec2 vtx_uv;varying vec2 coords[14];void main(){	vec4 depth1  = texture2D(srcBuf_depth,vtx_uv);	float factor = sqrt(depth1.x) - sqrt(param.x);	float dofblur = abs(factor * param.y*10.0);// * 0.3;	gl_FragColor = vec4(dofblur,dofblur,dofblur,1.0);	    //gl_FragColor = texture2D(srcBuf_depth, vtx_uv);}]]GeometryShader = ""DepthTest = false