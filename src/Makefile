#OPTS = -g
OPTS = -O3 -DHIGHRES_SHADOWS

INC_DIR = ../include

HOST_PLATFORM := $(shell $(CPP) -dumpmachine)
$(info $(HOST_PLATFORM))

# uncomment and fix for your system if nvidia under ubuntu 13.10
#NVIDIA_LIBS=L/usr/lib/nvidia-319-updates
#NVIDIA_LIBS=L/usr/lib/nvidia-331-updates

# settings for turing
ifeq   "$(HOST_PLATFORM)" "x86_64-redhat-linux"
INCLUDES = -I/opt/viz/include
LIBS = -lGL -lGLU -lglut -L/opt/viz/lib -lGLEW
endif

ifeq   "$(HOST_PLATFORM)" "x86_64-linux-gnu"
#for sn336,sn148, ubuntu 
INCLUDES = -I../include -I.
LIBS = $(NVIDIA_LIBS) -lGL -lGLU -lglut -lGLEW 
endif

ifeq   "$(HOST_PLATFORM)" "i686-linux-gnu"
#for 32-bit linux
INCLUDES = -I../include -I.
LIBS = -lGL -lGLU -lglut -lGLEW 
endif

ifeq "$(HOST_PLATFORM)" "aarch64-linux-gnu"
# for Jetson Nano 2GB
INCLUDES = -I../include -I.
LIBS = -lGL -lGLU -lglut -lGLEW
endif

# for macosx
# NOTE for mac you have to use the GLSL 1.2 shaders....
#ifeq   "$(HOST_PLATFORM)" "x86_64-apple-darwin11.4.2"
#ifeq   "$(HOST_PLATFORM)" "x86_64-apple-darwin13.0.2"
#ifeq   "$(HOST_PLATFORM)" "x86_64-apple-darwin13.1.0"
#ifeq   "$(HOST_PLATFORM)" "x86_64-apple-darwin13.3.0"
#ifeq   "$(HOST_PLATFORM)" "x86_64-apple-darwin13.4.0"

ifneq (,$(findstring apple, $(HOST_PLATFORM)))


INCLUDES = -I../include -framework OpenGL -framework GLUT -DUSE_APPLE_VERTEXARRAYS
LIBS =
endif

OBJS = InitShader.o \
	ObjLoader.o \
	Texture.o \
	TextureLoader.o \
	FrameBufferObject.o \
	ParticleSystem.o \
	ssg.o

SSG_INCS = $(INC_DIR)/ssg.h \
	$(INC_DIR)/_ssg.h \
	$(INC_DIR)/Ptr.h \
	$(INC_DIR)/CheckError.h \
	$(INC_DIR)/InitShader.h \
	$(INC_DIR)/Texture.h \
	$(INC_DIR)/TextureLoader.h \
	$(INC_DIR)/ObjLoader.h \
	$(INC_DIR)/FrameBufferObject.h \
	$(INC_DIR)/PointLight.h \
	$(INC_DIR)/ParticleSystem.h \
	$(INC_DIR)/Camera.h \
	$(INC_DIR)/Trackball.h 


all : 	ssg.a

ssg.o : ssg.cpp $(SSG_INCS)
	g++ $(OPTS) -c $(INCLUDES) ssg.cpp

InitShader.o : InitShader.cpp $(SSG_INCS)
	g++ $(OPTS) -c $(INCLUDES) InitShader.cpp

ObjLoader.o : ObjLoader.cpp $(SSG_INCS)
	g++ $(OPTS) -c $(INCLUDES) ObjLoader.cpp

Texture.o : Texture.cpp $(SSG_INCS)
	g++ $(OPTS) -c $(INCLUDES) Texture.cpp

TextureLoader.o : TextureLoader.cpp $(SSG_INCS)
	g++ $(OPTS) -c $(INCLUDES) TextureLoader.cpp

FrameBufferObject.o : FrameBufferObject.cpp $(SSG_INCS)
	g++ $(OPTS) -c $(INCLUDES) FrameBufferObject.cpp

ParticleSystem.o : ParticleSystem.cpp $(SSG_INCS)
	g++ $(OPTS) -c $(INCLUDES) ParticleSystem.cpp

ssg.a : $(OBJS)
	ar rvs ssg.a $(OBJS)

clean :
	rm -f *.o
	rm -f *~
	rm -rf *.dSYM
	rm -f ssg.a
