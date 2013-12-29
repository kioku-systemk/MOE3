
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
	{name="tdfall", path="scene/tdfall.MRZ"},
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
demotime[11] = {mpart*10, mpart*10.5}
print("alltime = " ..alltime[2].. " / calctime = " .. mpart*10.5);

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
    return {demotime={stime,etime}, src="plane", target={bufname},  shader="scene/bokehblur", tex={srcBuf=srcbuf},vec4={ep1="0.750,0.400,0.9,0.375"}}
end
function effectdof(stime,etime,bufname,srcbuf)
    return {demotime={stime,etime}, src="plane", target={bufname},  shader="scene/dof", tex={srcBuf=srcbuf}, vec4={size=w..","..h..",0,0"}}
end

function showdepth(stime,etime,bufname,srcbuf)
    return {demotime={stime,etime}, src="plane", target={bufname},  shader="scene/depthshow", tex={srcBuf=srcbuf}}
end

local gbsize = "0.8,0.8,0,0"
function effectGaussH(stime,etime,bufname,srcbuf)
    return {demotime={stime,etime}, src="plane", target={bufname},  shader="scene/gaussblurh", tex={srcBuf=srcbuf}, vec4={scale=gbsize}}
end
function effectGaussV(stime,etime,bufname,srcbuf)
    return {demotime={stime,etime}, src="plane", target={bufname},  shader="scene/gaussblurv", tex={srcBuf=srcbuf}, vec4={scale=gbsize}}
end

Process = {
	{demotime=alltime, scenetime=alltime, scene="tdfall"},
};

Render = {
    clear (alltime[1],alltime[2], "backbuffer"),
	render(alltime[1],alltime[2], "backbuffer","tdfall"),	
};

demotime = alltime;
print("LOADED: demo.lua")
