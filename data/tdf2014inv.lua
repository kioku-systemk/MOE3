
--- Demo project scene and post effect file

-- Default value
-- screen_width = 1920;
-- screen_heith = 1080;

local w = screen_width;
local h = screen_height;
local hw = screen_width/2;
local hh = screen_height/2;

soundfile = "music.mp3"

-- first, basic buffer is supported
Buffer = {
	--{name="backbuffer"} -- default buffer
	{name="buf1", color="RGBA8", depth="D24", width=screen_width, height=screen_height},
	{name="buf2", color="RGBA32F", depth="D24", width=screen_width, height=screen_height},
   	{name="buf3", color="RGBA8", depth="D24", width=screen_width, height=screen_height}
};

Scene = {
	{name="plane", path="scene/plane.MRZ"},
	{name="toukyou", path="scene/toukyou.MRZ"},
	{name="dropballsB", path="scene/dropballsB.MRZ"},
	{name="tdfandline", path="scene/tdfandline.MRZ"},
	{name="transsphere", path="scene/transsphere.MRZ"},
	{name="breakbox", path="scene/breakbox.MRZ"},
	{name="spherebreak", path="scene/spherebreak.MRZ"},
	{name="boxstageandline", path="scene/boxstageandline.MRZ"},
	{name="boxfly", path="scene/boxfly.MRZ"},
	{name="downringsB", path="scene/downringsB.MRZ"},
	{name="kaijyou", path="scene/kaijyou.MRZ"},
	{name="events", path="scene/events.MRZ"},
	{name="stripetestB", path="scene/stripetestB.MRZ"},
}

-- dependent flow is not support yet
-- ex. animation, physics, update partiles
local mpart = 21.57
demotime={}
alltime     = {0.0000,227.000}
demotime[1]  = {0.0000,  mpart*1-0.0001}
demotime[2]  = {mpart*1, mpart*2-0.0001}
demotime[3]  = {mpart*2, mpart*3-0.0001}
demotime[4]  = {mpart*3, mpart*4-0.0001}
demotime[5]  = {mpart*4, mpart*5-0.0001}
demotime[6]  = {mpart*5, mpart*6-0.0001}
demotime[7]  = {mpart*6, mpart*7-0.0001}
demotime[8]  = {mpart*7, mpart*8-0.0001}
demotime[9]  = {mpart*8, mpart*9-0.0001}
demotime[10] = {mpart*9, mpart*10-0.0001}
demotime[11] = {mpart*10, alltime[2]}

demotime["2a"]  = {mpart*1, mpart*1.5-0.0001}
demotime["2b"]  = {mpart*1.5, mpart*2-0.0001}


function clear(stime,etime,name)
    return {demotime={stime,etime}, src="clear", target={name}, vec4={color="0,0,0,1", flag="1,1,0,0"}}
end
function depthclear(stime,etime,bufname)
	return {demotime={stime,etime}  , src="depthclear", target={bufname}, vec4={flag="0,1,0,0"}}
end
function render(stime,etime,bufname, scenename)
    return {demotime={stime,etime}, src=scenename, target={bufname}, shader="default" }
end
function effectbokeh(stime,etime,bufname,srcbuf)
    return {demotime={stime,etime}, src="plane", target={bufname},  shader="fx/bokehblur", tex={srcBuf=srcbuf},vec4={ep1="0.750,0.400,0.9,0.375"}}
end
function effectdof(stime,etime,bufname,srcbuf)
    return {demotime={stime,etime}, src="plane", target={bufname},  shader="fx/dof", tex={srcBuf=srcbuf}, vec4={size=w..","..h..",0,0"}}
end

function showdepth(stime,etime,bufname,srcbuf)
    return {demotime={stime,etime}, src="plane", target={bufname},  shader="fx/depthshow", tex={srcBuf=srcbuf}}
end

local gbsize = "0.8,0.8,0,0"
function effectGaussH(stime,etime,bufname,srcbuf)
    return {demotime={stime,etime}, src="plane", target={bufname},  shader="fx/gaussblurh", tex={srcBuf=srcbuf}, vec4={scale=gbsize}}
end
function effectGaussV(stime,etime,bufname,srcbuf)
    return {demotime={stime,etime}, src="plane", target={bufname},  shader="fx/gaussblurv", tex={srcBuf=srcbuf}, vec4={scale=gbsize}}
end
	
Process = {
--	{demotime=alltime,        scenetime={0,5.000}, scene="plane"},
	{demotime=demotime[1],    scenetime={0,5.000}, scene="toukyou"},
	{demotime=demotime["2a"], scenetime={1,2.000}, scene="stripetestB"},
	{demotime=demotime["2b"], scenetime={0.5,1.500}, scene="stripetestB", camera="CameraA"},	
	{demotime=demotime[3],    scenetime={0,5.000}, scene="dropballsB"},
	{demotime=demotime[4],    scenetime={0,5.000}, scene="downringsB"},
	{demotime=demotime[5],    scenetime={0,5.000}, scene="breakbox"},
	{demotime=demotime[6],    scenetime={0,30.000},scene="events"},
	{demotime=demotime[7],    scenetime={0,5.000}, scene="boxfly"},
	{demotime=demotime[8],    scenetime={0,5.000}, scene="boxstageandline"},
	{demotime=demotime[9],    scenetime={0,5.000}, scene="transsphere"},
	{demotime=demotime[10],   scenetime={0,5.000}, scene="spherebreak",vec4={p1="1,1,1,1"}},
	{demotime=demotime[11],   scenetime={0,5.000}, scene="tdfandline"},
};

Render = {
    clear (0,227, "backbuffer"),
	render(demotime[1][1],demotime[1][2], "backbuffer","toukyou"),
	render(demotime[2][1],demotime[2][2], "backbuffer","stripetestB"),
	render(demotime[3][1],demotime[3][2], "backbuffer","dropballsB"),
	render(demotime[4][1],demotime[4][2], "backbuffer","downringsB"),
	render(demotime[5][1],demotime[5][2], "backbuffer","breakbox"),
	render(demotime[6][1],demotime[6][2], "backbuffer","events"),
	render(demotime[7][1],demotime[7][2], "backbuffer","boxfly"),
	render(demotime[8][1],demotime[8][2], "backbuffer","boxstageandline"),
	render(demotime[9][1],demotime[9][2], "backbuffer","transsphere"),
	render(demotime[10][1],demotime[10][2], "backbuffer","spherebreak"),
	render(demotime[11][1],demotime[11][2], "backbuffer","tdfandline"),	
};

demotime = alltime;
print("LOADED: demo.lua")
