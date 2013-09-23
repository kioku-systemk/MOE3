
--- Demo project scene and post effect file

-- Default value
-- screen_width = 1920;
-- screen_heith = 1080;

-- first, basic buffer is supported
Buffer = {
	--{name="backbuffer"} -- default buffer
	{name="buf1", color="RGBA", depth="D24", width=screen_width, height=scree_height},
	{name="buf2","color=RGBA", depth="D24", width=screen_width/2, height=scree_height/2}
};

Scene = {
	{name="plane",  "data/scene/plane.MRZ"},
	{name="scene1", "data/scene/scene1.MRZ"},
	{name="scene2", "data/scene/scene2.MRZ"},
	{name="scene3", "data/script/scene1.lua"}
}

-- dependent flow is not support yet
-- ex. animation, physics, update partiles
Process = {
	{demotime={0   ,5000}  , scenetime={0,5000}, scene="scene1"},
	{demotime={5000,10000} , scenetime={0,5000}, scene="scene2"},
	{demotime={10000,15000}, scenetime={0,5000}, scene="scene3"}
};

Render = {
	{demotime={0   ,5000}  , src="scene1", target={"buf1"},       shader="default"},
	{demotime={0   ,5000}  , src="plane",  target={"backbuffer"}, shader="copyShader", param={srcBuf="buf1"}},
	{demotime={5000,10000} , src="scene2", target={"backbuffer"}, shader="default"},
	{demotime={10000,15000}, src="scene3", target={"backbuffer"}, shader="default"}
};
