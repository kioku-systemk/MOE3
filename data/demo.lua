
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
	{name="plane",  path="scene/plane.MRZ"},
	{name="scene1", path="scene/breakbox.MRZ"},
	{name="scene2", path="scene/test.MRZ"},
   	{name="boxscat", path="scene/boxscat.MRZ"},
   	{name="fovanim", path="scene/fovanim.MRZ"},
    {name="grptest", path="scene/grptest.MRZ"},
--	{name="scene3", path="script/scene1.lua"}
}

-- dependent flow is not support yet
-- ex. animation, physics, update partiles
Process = {
	{demotime={0   ,5.000}  , scenetime={0,2.000}, scene="scene1",vec4={p1="1,1,1,1"}},
	{demotime={0   ,5.000}  , scenetime={0,5.000}, scene="plane"},
    {demotime={0   ,5.000}  , scenetime={0,5.000}, scene="grptest"},
	{demotime={5.000,7.000} , scenetime={0,5.000}, scene="scene2"},
	{demotime={5.000,7.000} , scenetime={0,2.000}, scene="fovanim"},
  	{demotime={7.000,10.000} , scenetime={0,2.000}, scene="grptest"},
--	{demotime={1.0000,15.000}, scenetime={0,5.000}, scene="scene3"}
};

function clear(stime,etime,name)
    return {demotime={stime,etime}, src="clear", target={name}, vec4={color="0,0,0,1", flag="1,1,0,0"}}
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
    clear (0,10, "buf2"),
    clear (0,10, "buf3"),
--    render(0,5 , "backbuffer","scene1"),
   	render(0,5 , "buf2","scene1"),
--    showdepth(0,5 ,"backbuffer", "buf2"),
    effectbokeh(0,5 ,"backbuffer", "buf2"),
--    effectGaussH(0,5 ,"buf3", "buf2"),
--    effectGaussV(0,5 ,"backbuffer", "buf3"),
--    {demotime={0   ,5.000}  , src="depthclear", target={"backbuffer"}, vec4={flag="0,1,0,0"}},
--	{demotime={0   ,3.000}  , src="scene1", target={"backbuffer"}, shader="default"},
--	{demotime={0   ,5.000}  , src="plane",  target={"backbuffer"}, shader="copyShader"},
	render(5,7 ,"backbuffer", "scene2"),
    render(7,10,"backbuffer", "grptest")
};

print(screen_width);
