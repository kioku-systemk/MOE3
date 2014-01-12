
VertexShader = [[
attribute vec3 pos;
attribute vec4 col;
attribute vec3 nor;
attribute vec2 uv;
uniform sampler2D color_tex;
uniform mat4 proj;
uniform mat4 view;
uniform mat4 world;
uniform vec4 p1;
varying HIGHP vec4 vtx_pos;
varying HIGHP vec2 vtx_uv;
varying LOWP vec4    vtx_col;

void main(void)
{
	vtx_uv = uv;
	vtx_col = col;
	vtx_pos = world * vec4(pos, 1);
	gl_Position = proj * view * vtx_pos;
}
]]

FragmentShader = [[
	varying HIGHP vec4 vtx_pos;
	uniform vec4 p1;
	uniform vec4 eyepos;
uniform vec4 time;
varying HIGHP vec2 vtx_uv;
	/*uniform sampler2D color_tex; 
uniform vec4 color;
uniform vec4 time;
uniform vec4 p1;
varying HIGHP vec2 vtx_uv;
varying LOWP vec4  vtx_col;
float PI = 3.1415926;
void main(void)
{
	gl_FragColor = vec4(1, 1, 0, 1);s// color * texture2D(color_tex, vtx_uv);
}*/

	float map(vec3 p)
	{
		const int MAX_ITER = 20;
		const float BAILOUT = 4.0;
		float Power = 10.0+sin(time.x*2.0)*1.0;

		vec3 v = p;
		vec3 c = v;

		float r = 0.0;
		float d = 1.0;
		for (int n = 0; n <= MAX_ITER; ++n)
		{
			r = length(v);
			if (r>BAILOUT) break;

			float theta = acos(v.z / r);
			float phi = atan(v.y, v.x);
			d = pow(r, Power - 1.0)*Power*d + 1.0;

			float zr = pow(r, Power);
			theta = theta*Power;
			phi = phi*Power;
			v = (vec3(cos(theta)*sin(phi), cos(phi)*sin(theta), sin(theta))*zr) + c;
		}
		return 0.5*log(r)*r / d;
	}

	
	void main(void)
	{
		vec2 pos = vtx_pos.xy;// (gl_FragCoord.xy*3.0 - resolution.xy) / resolution.y;
		//pos -= mouse;
		vec3 camPos = vec3(cos(time.x*0.2), sin(time.x*0.2), 1.5);
		vec3 camTarget = vec3(0.0, 0.0, 0.0);

		vec3 camDir = normalize(camTarget - camPos);
		vec3 camUp = normalize(vec3(0.0, 1.0, 0.0));
		vec3 camSide = cross(camDir, camUp);
		float focus = 1.8;

		vec3 rayDir = normalize(vec3(vtx_pos.xyz-eyepos.xyz));// normalize(camSide*pos.x + camUp*pos.y + camDir*focus);
		
		vec3 ray = vtx_pos.xyz*0.15 + p1.x;// camPos;
		vec3 m = vec3(0.0);
		float d = 0.0, total_d = 0.0;
		const int MAX_MARCH = 50;
		const float MAX_DISTANCE = 1000.0;
		for (int i = 0; i<MAX_MARCH; ++i) {
			d = map(ray);
			total_d += d;
			ray += rayDir * d;
			m += vec3(1.3,1.1,2.2);
			if (d<0.001) { break; }
			if (total_d>MAX_DISTANCE) { total_d = MAX_DISTANCE; discard; }
		}

		float c = (total_d)*0.0001;
		vec4 result = vec4(1.0 - vec3(c, c, c) - vec3(0.025, 0.025, 0.025)*m*0.9, 1.0);
		float cld = total_d;// + 
		float ex = sin(vtx_uv.x+time.x*0.10)+0.15;
		if (time.x > 4.0)
			ex = 0.0;
		vec3 cm = vec3(sin(cld*10.0)*0.1+0.8+ex, sin(cld*20.0)*0.1+0.8, sin(cld*30.0)*0.1+0.5);
		result.xyz *= cm;
		gl_FragColor = result*4.0;
		gl_FragColor.a = 1.0;
	}

]]

GeometryShader = ""

DepthTest = true
Blend = true
BlendSrc = "SRC_ALPAH"
BlendDst = "ONE_MINUS_SRC_ALPHA"

Priority=10