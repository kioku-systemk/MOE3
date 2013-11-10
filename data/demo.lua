
--- Demo project scene and post effect file

-- Default value
-- screen_width = 1920;
-- screen_heith = 1080;

print(screen_width);

-- first, basic buffer is supported
Buffer = {
	--{name="backbuffer"} -- default buffer
	{name="buf1", color="RGBA8", depth="D24", width=screen_width, height=screen_height},
	{name="buf2", color="RGBA8", depth="D24", width=screen_width/8, height=screen_height/8}
};

Scene = {
	{name="plane",  path="scene/plane.MRZ"},
	{name="scene1", path="scene/spherebreak.MRZ"},
	{name="scene2", path="scene/test.MRZ"},
   	{name="boxscat", path="scene/boxscat.MRZ"},
   	{name="fovanim", path="scene/fovanim.MRZ"},
--	{name="scene3", path="script/scene1.lua"}
}

-- dependent flow is not support yet
-- ex. animation, physics, update partiles
Process = {
	{demotime={0   ,3.000}  , scenetime={0,5.000}, scene="scene1",vec4={scolor="1,0,0,1"}},
	{demotime={0   ,5.000}  , scenetime={0,5.000}, scene="plane"},
    {demotime={0   ,5.000}  , scenetime={0,5.000}, scene="boxscat"},
	{demotime={2.000,5.000} , scenetime={0,5.000}, scene="scene2"},
	{demotime={5.000,7.000} , scenetime={0,2.000}, scene="fovanim"},
--	{demotime={1.0000,15.000}, scenetime={0,5.000}, scene="scene3"}
};

Render = {
    {demotime={0   ,5.000}  , src="clear", target={"buf2"}, vec4={color="1,1,1,1", flag="1,1,0,0"}},
   	{demotime={0   ,5.000}  , src="boxscat", target={"buf2"}, shader="default"},
    {demotime={0   ,5.000}  , src="plane", target={"backbuffer"},  shader="scene/boxblur",tex={srcBuf="buf2"},
        vec4={size=(screen_width/8)..","..(screen_height/8)..",0,0"}},
    {demotime={0   ,5.000}  , src="depthclear", target={"backbuffer"}, vec4={flag="0,1,0,0"}},

	{demotime={0   ,3.000}  , src="scene1", target={"backbuffer"}, shader="default"},

--	{demotime={0   ,5.000}  , src="plane",  target={"backbuffer"}, shader="copyShader"},
	{demotime={3.000,5.000} , src="scene2", target={"backbuffer"}, shader="default"},
	{demotime={5   ,7.000}  , src="fovanim", target={"backbuffer"}, shader="default"},
--	{demotime={10.000,15.000}, src="scene3", target={"backbuffer"}, shader="default"}
};
