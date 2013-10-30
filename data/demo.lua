
--- Demo project scene and post effect file

-- Default value
-- screen_width = 1920;
-- screen_heith = 1080;

print(screen_width);

-- first, basic buffer is supported
Buffer = {
	--{name="backbuffer"} -- default buffer
	{name="buf1", color="RGBA8", depth="D24", width=screen_width, height=screen_height},
	{name="buf2", color="RGBA8", depth="D24", width=screen_width/2, height=screen_height/2}
};

Scene = {
	{name="plane",  path="scene/plane.MRZ"},
	{name="scene1", path="scene/spherebreak.MRZ"},
	{name="scene2", path="scene/test.MRZ"},
--	{name="scene3", path="script/scene1.lua"}
}

-- dependent flow is not support yet
-- ex. animation, physics, update partiles
Process = {
	{demotime={0   ,3.000}  , scenetime={0,5.000}, scene="scene1"},
	{demotime={0   ,3.000}  , scenetime={0,5.000}, scene="plane"},
	{demotime={2.000,5.000} , scenetime={0,5.000}, scene="scene2"},
--	{demotime={1.0000,15.000}, scenetime={0,5.000}, scene="scene3"}
};

Render = {
	{demotime={0   ,3.000}  , src="scene1", target={"buf1"}, shader="default"},
    {demotime={0   ,3.000}  , src="plane", target={"backbuffer"},  shader="scene/screen",param={{"srcBuf","buf1"}}},

--	{demotime={0   ,5.000}  , src="plane",  target={"backbuffer"}, shader="copyShader", param={{"srcBuf","buf1"}}},
	{demotime={2.000,5.000} , src="scene2", target={"backbuffer"}, shader="default"},
--	{demotime={10.000,15.000}, src="scene3", target={"backbuffer"}, shader="default"}
};
