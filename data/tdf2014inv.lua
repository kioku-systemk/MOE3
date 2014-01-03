
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
	{name="hdrbuffer", color="RGBA32F", depth="D24", width=screen_width, height=screen_height},
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



function clear(stime,etime,name)
    return {demotime={stime,etime}, src="clear", target={name}, vec4={color="0,0,0,1", flag="1,1,0,0"}}
end
function depthclear(stime,etime,bufname)
	return {demotime={stime,etime}  , src="depthclear", target={bufname}, vec4={flag="0,1,0,0"}}
end
function render(stime,etime,bufname, scenename)
    return {demotime={stime,etime}, src=scenename, target={bufname}, shader="default" }
end
function renderPlaneShadow(stime,etime,bufname, scenename)
    return {demotime={stime,etime}, src=scenename, target={bufname}, shader="fx/planeshadow" }
end

--function effectbokeh(stime,etime,bufname,srcbuf)
--    return {demotime={stime,etime}, src="plane", target={bufname},  shader="fx/bokehblur", tex={srcBuf=srcbuf},vec4={ep1="0.812,0.550,0.15,0.712"}}
--end
function effectdof(stime,etime,bufname,srcbuf,ep)
    return {demotime={stime,etime}, src="plane", target={bufname},  shader="fx/dof", tex={srcBuf=srcbuf}, vec4={size=w..","..h..",0,0", ep1=ep}, }
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
	
	
-- dependent flow is not support yet
-- ex. animation, physics, update partiles
local mpart = 21.57
demotime={}
alltime      = {0.0000,227.000}
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

demotime["2a"]  = {mpart*1.00, mpart*1.25-0.0001}
demotime["2b"]  = {mpart*1.25, mpart*1.50-0.0001}
demotime["2c"]  = {mpart*1.50, mpart*1.75-0.0001}
demotime["2d"]  = {mpart*1.75, mpart*2.00-0.0001}

-- 3
local sneat = 0.03125*6
local snea1 = 0.5
local snea2 = snea1 + sneat
local snea3 = 1.0

local q
for q = 3,5 do
local stm = q-1.0
	demotime[q.."a"]  = {mpart*(stm+0.00), mpart*(stm+0.25) -0.0001}
	demotime[q.."b"]  = {mpart*(stm+0.25), mpart*(stm+0.50) -0.0001}
	demotime[q.."c"]  = {mpart*(stm+0.50), mpart*(stm+0.75) -0.0001}
	demotime[q.."d"]  = {mpart*(stm+0.75), mpart*(stm+1.00) -0.0001}
	demotime[q.."a1"]  = {mpart*stm+mpart*0.25*0.000, mpart*stm+mpart*0.25*snea1 -0.0001}
	demotime[q.."a2"]  = {mpart*stm+mpart*0.25*snea1, mpart*stm+mpart*0.25*snea2 -0.0001}
	demotime[q.."a3"]  = {mpart*stm+mpart*0.25*snea2, mpart*stm+mpart*0.25*snea3 -0.0001}
	demotime[q.."b1"]  = {mpart*(stm+0.25)+mpart*0.25*0.000, mpart*(stm+0.25)+mpart*0.25*snea1 -0.0001}
	demotime[q.."b2"]  = {mpart*(stm+0.25)+mpart*0.25*snea1, mpart*(stm+0.25)+mpart*0.25*snea2 -0.0001}
	demotime[q.."b3"]  = {mpart*(stm+0.25)+mpart*0.25*snea2, mpart*(stm+0.25)+mpart*0.25*snea3 -0.0001}
	demotime[q.."c1"]  = {mpart*(stm+0.50)+mpart*0.25*0.000, mpart*(stm+0.50)+mpart*0.25*snea1 -0.0001}
	demotime[q.."c2"]  = {mpart*(stm+0.50)+mpart*0.25*snea1, mpart*(stm+0.50)+mpart*0.25*snea2 -0.0001}
	demotime[q.."c3"]  = {mpart*(stm+0.50)+mpart*0.25*snea2, mpart*(stm+0.50)+mpart*0.25*snea3 -0.0001}
	demotime[q.."d1"]  = {mpart*(stm+0.75)+mpart*0.25*0.000, mpart*(stm+0.75)+mpart*0.25*snea1 -0.0001}
	demotime[q.."d2"]  = {mpart*(stm+0.75)+mpart*0.25*snea1, mpart*(stm+0.75)+mpart*0.25*snea2 -0.0001}
	demotime[q.."d3"]  = {mpart*(stm+0.75)+mpart*0.25*snea2, mpart*(stm+0.75)+mpart*0.25*snea3 -0.0001}
end

p4={
	{1.375,1.542},
	{1.583,2.000},
	{1.625,2.000},
	{1.667,2.000}
}
for i,v in pairs(p4) do v[3] = v[2]-v[1] end

p5={
	{0.000,1.000},
	{0.500,2.000},
	{0.750,1.375},
	{0.800,1.250}
}
for i,v in pairs(p5) do v[3] = v[2]-v[1] end


demotime["10a"]  = {mpart*9.00, mpart*9.25-0.0001}
demotime["10b"]  = {mpart*9.25, mpart*9.50-0.0001}
demotime["10c"]  = {mpart*9.50, mpart*9.75-0.0001}
demotime["10d"]  = {mpart*9.75, mpart*10.0-0.0001}

	
Process = {
	{demotime=alltime,        scene="plane",   scenetime={0,5.000}},
	{demotime=demotime[1],    scene="toukyou", scenetime={0,5.000}},
	{demotime=demotime["2a"], scene="stripetestB", scenetime={0.000+0.1375*0.5,1.375*0.5}, camera="CameraB"},	
	{demotime=demotime["2b"], scene="stripetestB", scenetime={0.2500+0.2,1.750*0.5}, camera="Camera" },
	{demotime=demotime["2c"], scene="stripetestB", scenetime={0.2500+0.2,1.500*0.5}, camera="CameraA"},
	{demotime=demotime["2d"], scene="stripetestB", scenetime={0.2500+0.2,1.500*0.5}, camera="CameraC"},
	
	{demotime=demotime["3a1"], scene="dropballsB", scenetime={1.0      ,1.5      }, camera="Camera"  },
	{demotime=demotime["3a2"], scene="dropballsB", scenetime={1.50     ,1.50+0.01}, camera="Camera"  },
	{demotime=demotime["3a3"], scene="dropballsB", scenetime={1.50+0.01,1.75+0.01}, camera="Camera"  },
	{demotime=demotime["3b1"], scene="dropballsB", scenetime={1.0      ,1.5      }, camera="CameraA"  },
	{demotime=demotime["3b2"], scene="dropballsB", scenetime={1.50     ,1.50+0.01}, camera="CameraA"  },
	{demotime=demotime["3b3"], scene="dropballsB", scenetime={1.50+0.01,1.75+0.01}, camera="CameraA"  },
	{demotime=demotime["3c1"], scene="dropballsB", scenetime={1.0      ,1.5      }, camera="CameraB"  },
	{demotime=demotime["3c2"], scene="dropballsB", scenetime={1.50     ,1.50+0.01}, camera="CameraB"  },
	{demotime=demotime["3c3"], scene="dropballsB", scenetime={1.50+0.01,1.75+0.01}, camera="CameraB"  },
	{demotime=demotime["3d1"], scene="dropballsB", scenetime={1.0      ,1.5      }, camera="CameraC"  },
	{demotime=demotime["3d2"], scene="dropballsB", scenetime={1.50     ,1.50+0.01}, camera="CameraC"  },
	{demotime=demotime["3d3"], scene="dropballsB", scenetime={1.50+0.01,1.75+0.01}, camera="CameraC"  },
	
--	{demotime=demotime["4a"], scene="downringsB", scenetime={1.375,1.542}, camera="CameraA" },
--	{demotime=demotime["4b"], scene="downringsB", scenetime={1.583,2.000}, camera="CameraB" },
--	{demotime=demotime["4c"], scene="downringsB", scenetime={1.625,2.000}, camera="CameraC" },
--	{demotime=demotime["4d"], scene="downringsB", scenetime={1.667,2.000}, camera="Camera"  },	
	{demotime=demotime["4a1"], scene="downringsB", scenetime={p4[1][1]                   ,p4[1][1]+0.50*p4[1][3]      }, camera="CameraA"  },
	{demotime=demotime["4a2"], scene="downringsB", scenetime={p4[1][1]+0.5*p4[1][3]      ,p4[1][1]+0.50*p4[1][3]+0.005}, camera="CameraA"  },
	{demotime=demotime["4a3"], scene="downringsB", scenetime={p4[1][1]+0.5*p4[1][3]+0.005,p4[1][1]+0.75*p4[1][3]+0.005}, camera="CameraA"  },
	{demotime=demotime["4b1"], scene="downringsB", scenetime={p4[2][1]                   ,p4[2][1]+0.50*p4[2][3]      }, camera="CameraB"  },
	{demotime=demotime["4b2"], scene="downringsB", scenetime={p4[2][1]+0.5*p4[2][3]      ,p4[2][1]+0.50*p4[2][3]+0.005}, camera="CameraB"  },
	{demotime=demotime["4b3"], scene="downringsB", scenetime={p4[2][1]+0.5*p4[2][3]+0.005,p4[2][1]+0.75*p4[2][3]+0.005}, camera="CameraB"  },
	{demotime=demotime["4c1"], scene="downringsB", scenetime={p4[3][1]                   ,p4[3][1]+0.50*p4[3][3]      }, camera="CameraC"  },
	{demotime=demotime["4c2"], scene="downringsB", scenetime={p4[3][1]+0.5*p4[3][3]      ,p4[3][1]+0.50*p4[3][3]+0.005}, camera="CameraC"  },
	{demotime=demotime["4c3"], scene="downringsB", scenetime={p4[3][1]+0.5*p4[3][3]+0.005,p4[3][1]+0.75*p4[3][3]+0.005}, camera="CameraC"  },
	{demotime=demotime["4d1"], scene="downringsB", scenetime={p4[4][1]                   ,p4[4][1]+0.50*p4[4][3]      }, camera="Camera"  },
	{demotime=demotime["4d2"], scene="downringsB", scenetime={p4[4][1]+0.5*p4[4][3]      ,p4[4][1]+0.50*p4[4][3]+0.005}, camera="Camera"  },
	{demotime=demotime["4d3"], scene="downringsB", scenetime={p4[4][1]+0.5*p4[4][3]+0.005,p4[4][1]+0.75*p4[4][3]+0.005}, camera="Camera"  },
	
	{demotime=demotime["5a"], scene="breakbox", scenetime={p5[1][1]                   ,p5[1][2]      }, camera="CameraC"  },
--	{demotime=demotime["5a1"], scene="breakbox", scenetime={p5[1][1]                   ,p5[1][1]+0.50*p5[1][3]      }, camera="CameraC"  },
--	{demotime=demotime["5a2"], scene="breakbox", scenetime={p5[1][1]+0.5*p5[1][3]      ,p5[1][1]+0.50*p5[1][3]-0.200}, camera="CameraC"  },
--	{demotime=demotime["5a3"], scene="breakbox", scenetime={p5[1][1]+0.5*p5[1][3]-0.200,p5[1][1]+0.75*p5[1][3]      }, camera="CameraC"  },
	{demotime=demotime["5b"], scene="breakbox", scenetime={p5[2][1]                   ,p5[2][2]      }, camera="CameraA"  },
--	{demotime=demotime["5b1"], scene="breakbox", scenetime={p5[2][1]                   ,p5[2][1]+0.50*p5[2][3]      }, camera="CameraA"  },
--	{demotime=demotime["5b2"], scene="breakbox", scenetime={p5[2][1]+0.5*p5[2][3]      ,p5[2][1]+0.50*p5[2][3]-0.200}, camera="CameraA"  },
--	{demotime=demotime["5b3"], scene="breakbox", scenetime={p5[2][1]+0.5*p5[2][3]-0.200,p5[2][1]+0.75*p5[2][3]      }, camera="CameraA"  },
	{demotime=demotime["5c1"], scene="breakbox", scenetime={p5[3][1]                   ,p5[3][1]+0.50*p5[3][3]      }, camera="CameraB"  },
	{demotime=demotime["5c2"], scene="breakbox", scenetime={p5[3][1]+0.5*p5[3][3]      ,p5[3][1]+0.50*p5[3][3]-0.500}, camera="CameraB"  },
	{demotime=demotime["5c3"], scene="breakbox", scenetime={p5[3][1]+0.5*p5[3][3]-0.500,p5[3][1]+0.75*p5[3][3]      }, camera="CameraB"  },
	{demotime=demotime["5d1"], scene="breakbox", scenetime={p5[4][1]                   ,p5[4][1]+0.50*p5[4][3]      }, camera="Camera"  },
	{demotime=demotime["5d2"], scene="breakbox", scenetime={p5[4][1]+0.5*p5[4][3]      ,p5[4][1]+0.50*p5[4][3]+0.005}, camera="Camera"  },
	{demotime=demotime["5d3"], scene="breakbox", scenetime={p5[4][1]+0.5*p5[4][3]+0.005,p5[4][1]+0.75*p5[4][3]+0.005}, camera="Camera"  },

	{demotime=demotime[6],    scenetime={0,21.570},scene="transsphere"},
	{demotime=demotime[7],    scenetime={0,21.570}, scene="boxfly"},
	{demotime=demotime[8],    scenetime={0,21.570}, scene="boxstageandline"},
	{demotime=demotime[9],    scenetime={0,21.570}, scene="events"},
	{demotime=demotime["10a"],   scene="spherebreak",scenetime={8.000,5.000}, camera="CameraB", vec4={p1="1,1,1,1"}},
	{demotime=demotime["10b"],   scene="spherebreak",scenetime={5.000,2.000}, camera="CameraA", vec4={p1="1,1,1,1"}},
	{demotime=demotime["10c"],   scene="spherebreak",scenetime={2.000,0.500}, camera="CameraC", vec4={p1="1,1,1,1"}},
	{demotime=demotime["10d"],   scene="spherebreak",scenetime={0.500,-0.025}, camera="CameraA", vec4={p1="1,1,1,1"}},
	
	{demotime=demotime[11],   scenetime={0,5.000}, scene="tdfandline"},
};

Render = {
	clear (0,227, "backbuffer"),
-- Scene1
	render(demotime[1][1],demotime[1][2], "backbuffer","toukyou"),
-- Scene2	
	clear    (demotime[2][1],demotime[2][2], "hdrbuffer"),
	render   (demotime[2][1],demotime[2][2], "hdrbuffer","stripetestB"),
	effectdof(demotime[2][1],demotime[2][2], "backbuffer","hdrbuffer","0.950,0.450,0.225,0.663"),
-- Scene3
	clear            (demotime[3][1],demotime[3][2], "hdrbuffer"),
	render           (demotime[3][1],demotime[3][2], "hdrbuffer","dropballsB"),
	renderPlaneShadow(demotime[3][1],demotime[3][2], "hdrbuffer","dropballsB"),
	effectdof(demotime[3][1],demotime[3][2],"backbuffer","hdrbuffer","0.938,0.488,0.613,0.1"),
	
-- Scene4	
	clear    (demotime[4][1],demotime[4][2], "hdrbuffer"),
	render   (demotime[4][1],demotime[4][2], "hdrbuffer","downringsB"),
	effectdof(demotime[4][1],demotime[4][2],"backbuffer","hdrbuffer","0.812,0.550,0.15,0.712"),
-- Scene5
	clear    (demotime[5][1],demotime[5][2], "hdrbuffer"),
	render   (demotime[5][1],demotime[5][2], "hdrbuffer","breakbox"),
	effectdof(demotime[5][1],demotime[5][2], "backbuffer","hdrbuffer","0.887,0.463,0.387,0.1"),
-- Scene6
	render(demotime[6][1],demotime[6][2], "backbuffer","transsphere"),
	render(demotime[7][1],demotime[7][2], "backbuffer","boxfly"),
	render(demotime[8][1],demotime[8][2], "backbuffer","boxstageandline"),
	render(demotime[9][1],demotime[9][2], "backbuffer","events"),
	render(demotime[10][1],demotime[10][2], "backbuffer","spherebreak"),
	render(demotime[11][1],demotime[11][2], "backbuffer","tdfandline"),	
};

demotime = alltime;
print("LOADED: demo.lua")
