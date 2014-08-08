

TARGET=demo.html

CXX=emcc

#ALLOWMEM =  -s ALLOW_MEMORY_GROWTH=1
MEM=-s TOTAL_MEMORY=268435456 -s NO_EXIT_RUNTIME=1 $(ALLOWMEM)
INCLUDE=-Ilib/include
FLAG=-O2 -std=c++11 $(INCLUDE) -D__EMSCRIPTEN__=1 -DUSE_GLEXTFUNC -DASM_JS=1 $(MEM)
LIB=lib/bc/liblua.bc lib/bc/libpng.bc lib/bc/zlib.bc
RES=--preload-file webdata


SRC  =src/Gfx/RenderDevice.o \
	src/SceneGraph/AnimLoader.o \
	src/demoplayer.o \
	src/Core/KdbImporterExporter.o \
	src/Demo/Scene.o \
	src/Demo/Demo.o \
	src/SceneGraph/MrzLoader.o \
	src/Core/jpgd.o \
	src/Demo/EffectBuffer.o \
	src/Core/Image.o \
	src/Core/Log.o \
	src/Core/Stream.o \
	src/Gfx/BufferObject.o \
	src/Gfx/ShaderProgramObject.o \
	src/Gfx/RenderCommand.o \
	src/Gfx/FrameBufferObject.o \
	src/Gfx/SceneGraphRender.o \
	src/Gfx/TextureObject.o \
	src/Core/CoreWindow_egl.o

%.o : %.cpp
	$(CXX) $(FLAG) -o $@ -c $<

$(TARGET) : $(SRC)
	$(CXX) $(SRC) $(LIB) $(RES) $(MEM) -o $(TARGET)

all: $(TARGET)

clean:
	rm $(TARGET); rm src/*.o; rm src/Gfx/*.o; rm src/Demo/*.o; rm src/Core/*.o

