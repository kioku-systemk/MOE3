
--- Demo project scene and post effect file

-- Default value
-- screen_width = 1920;
-- screen_heith = 1080;

local w = screen_width;
local h = screen_height;
local hw = screen_width/2;
local hh = screen_height/2;

-- first, basic buffer is supported
Buffer = {
	--{name="backbuffer"} -- default buffer
	{name="buf1", color="RGBA8", depth="D24", width=screen_width, height=screen_height},
	{name="buf2", color="RGBA16F", depth="D24", width=screen_width, height=screen_height},
   	{name="buf3", color="RGBA8", depth="D24", width=screen_width, height=screen_height}
};

Scene = {
	{name="plane", path="scene/plane.MRZ"},
	{name="tdfandline", path="scene/tdfandline.MRZ"},
	{name="breakbox", path="scene/breakbox.MRZ"},
	{name="spherebreak", path="scene/spherebreak.MRZ"},
	{name="boxstageandline", path="scene/boxstageandline.MRZ"},
	{name="sphereandlines", path="scene/sphereandlines.MRZ"},
}

-- dependent flow is not support yet
-- ex. animation, physics, update partiles
Process = {
	{demotime={0.000,100.000}, scenetime={0,5.000}, scene="plane"},
	{demotime={0.000, 5.000}, scenetime={0,5.000}, scene="spherebreak",vec4={p1="1,1,1,1"}},
	{demotime={5.000, 10.000}, scenetime={0,5.000}, scene="tdfandline"},
	{demotime={10.000,15.000}, scenetime={0,5.000}, scene="boxstageandline"},
	{demotime={15.000,20.000}, scenetime={0,5.000}, scene="sphereandlines"},
};

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
    return {demotime={stime,etime}, src="plane", target={bufname},  shader="scene/bokehblur", tex={srcBuf=srcbuf}}
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

Render = {
    clear (0,20, "buf2"),
	render(0,5 , "buf2","spherebreak"), effectbokeh(0,5 ,"backbuffer", "buf2"),
	render(5,10 , "buf2","tdfandline"), effectbokeh(5,10 ,"backbuffer", "buf2"),
	render(10,15 , "buf2","boxstageandline"), effectbokeh(10,15 ,"backbuffer", "buf2"),
	render(15,20 , "buf2","sphereandlines"), effectbokeh(15,20 ,"backbuffer", "buf2"),
};

demotime = 20;

print(screen_width);
