#OPTS = -g
OPTS = -O3

INC_DIR=../include
LIB_DIR=../src

HOST_PLATFORM := $(shell $(CPP) -dumpmachine)
$(info $(HOST_PLATFORM))

# settings for turing
ifeq   "$(HOST_PLATFORM)" "x86_64-redhat-linux"
INCLUDES = -I/opt/viz/include
LIBS = -lGL -lGLU -lglut -L/opt/viz/lib -lGLEW
endif

ifeq   "$(HOST_PLATFORM)" "x86_64-linux-gnu"
#for sn336,sn148, ubuntu 
INCLUDES = -I$(INC_DIR) 
LIBS = -lGL -lGLU -lglut -lGLEW 
endif

# for macosx
# NOTE for mac you have to use the GLSL 1.2 shaders....
#ifeq   "$(HOST_PLATFORM)" "x86_64-apple-darwin11.4.2"
ifeq   "$(HOST_PLATFORM)" "x86_64-apple-darwin13.0.0"
# we are disabling warnings (-w)to not get the GLUT deprecations warnings in Mavericks..
INCLUDES = -I$(INC_DIR) -framework OpenGL -framework GLUT -DUSE_APPLE_VERTEXARRAYS -w
LIBS =
endif


SSG_INCS = $(INC_DIR)/ssg.h \
	$(INC_DIR)/CheckError.h \
	$(INC_DIR)/InitShader.h \
	$(INC_DIR)/Texture.h \
	$(INC_DIR)/TextureLoader.h \
	$(INC_DIR)/ObjLoader.h \
	$(INC_DIR)/FrameBufferObject.h \
	$(INC_DIR)/ParticleSystem.h \
	$(INC_DIR)/Camera.h \
	$(INC_DIR)/Trackball.h 


all : 	example0 \
	example1 \
	example2 \
	example3 \
	example4 \
	example5 \
	example6 \
	example7 \
	example8 \
	example9 \
	example10 \
	depthrender \
	gimbal-lock \
	postprocess \
	discardfragments

example0 : example0.cpp $(SSG_INCS) $(LIB_DIR)/ssg.a
	g++ $(OPTS) -o example0 $(INCLUDES) example0.cpp $(LIB_DIR)/ssg.a $(LIBS)

example1 : example1.cpp $(SSG_INCS) $(LIB_DIR)/ssg.a
	g++ $(OPTS) -o example1 $(INCLUDES) example1.cpp $(LIB_DIR)/ssg.a $(LIBS)

example2 : example2.cpp $(SSG_INCS) $(LIB_DIR)/ssg.a
	g++ $(OPTS) -o example2 $(INCLUDES) example2.cpp $(LIB_DIR)/ssg.a $(LIBS)

example3 : example3.cpp $(SSG_INCS) $(INC_DIR)/Trackball.h $(LIB_DIR)/ssg.a 
	g++ $(OPTS) -o example3 $(INCLUDES) example3.cpp $(LIB_DIR)/ssg.a $(LIBS)

example4 : example4.cpp $(SSG_INCS) $(INC_DIR)/Trackball.h $(INC_DIR)/Camera.h $(LIB_DIR)/ssg.a 
	g++ $(OPTS) -o example4 $(INCLUDES) example4.cpp $(LIB_DIR)/ssg.a $(LIBS)

example5 : example5.cpp $(SSG_INCS) $(INC_DIR)/Trackball.h $(LIB_DIR)/ssg.a 
	g++ $(OPTS) -o example5 $(INCLUDES) example5.cpp $(LIB_DIR)/ssg.a $(LIBS)

example6 : example6.cpp $(SSG_INCS) $(INC_DIR)/Trackball.h $(INC_DIR)/Camera.h $(LIB_DIR)/ssg.a 
	g++ $(OPTS) -o example6 $(INCLUDES) example6.cpp $(LIB_DIR)/ssg.a $(LIBS)

example7 : example7.cpp $(SSG_INCS) $(INC_DIR)/Trackball.h $(INC_DIR)/Camera.h $(LIB_DIR)/ssg.a 
	g++ $(OPTS) -o example7 $(INCLUDES) example7.cpp $(LIB_DIR)/ssg.a $(LIBS)

example8 : example8.cpp $(SSG_INCS) $(INC_DIR)/Trackball.h $(INC_DIR)/Camera.h $(LIB_DIR)/ssg.a 
	g++ $(OPTS) -o example8 $(INCLUDES) example8.cpp $(LIB_DIR)/ssg.a $(LIBS)

example9 : example9.cpp $(SSG_INCS) $(INC_DIR)/Trackball.h $(INC_DIR)/Camera.h $(LIB_DIR)/ssg.a 
	g++ $(OPTS) -o example9 $(INCLUDES) example9.cpp $(LIB_DIR)/ssg.a $(LIBS)

example10 : example10.cpp $(SSG_INCS) $(INC_DIR)/Trackball.h $(INC_DIR)/Camera.h $(LIB_DIR)/ssg.a 
	g++ $(OPTS) -o example10 $(INCLUDES) example10.cpp $(LIB_DIR)/ssg.a $(LIBS)

depthrender : depthrender.cpp $(SSG_INCS) $(INC_DIR)/Trackball.h $(INC_DIR)/Camera.h $(LIB_DIR)/ssg.a 
	g++ $(OPTS) -o depthrender $(INCLUDES) depthrender.cpp $(LIB_DIR)/ssg.a $(LIBS)

gimbal-lock : gimbal-lock.cpp $(SSG_INCS) $(INC_DIR)/Trackball.h $(INC_DIR)/Camera.h $(LIB_DIR)/ssg.a 
	g++ $(OPTS) -o gimbal-lock $(INCLUDES) gimbal-lock.cpp $(LIB_DIR)/ssg.a $(LIBS)

postprocess : postprocess.cpp $(SSG_INCS) $(INC_DIR)/Trackball.h $(INC_DIR)/Camera.h $(LIB_DIR)/ssg.a 
	g++ $(OPTS) -o postprocess $(INCLUDES) postprocess.cpp $(LIB_DIR)/ssg.a $(LIBS)

discardfragments : discardfragments.cpp $(SSG_INCS) $(INC_DIR)/Trackball.h $(INC_DIR)/Camera.h $(LIB_DIR)/ssg.a 
	g++ $(OPTS) -o discardfragments $(INCLUDES) discardfragments.cpp $(LIB_DIR)/ssg.a $(LIBS)


clean :
	rm -f *.o
	rm -f *~
	rm -f example0
	rm -f example1
	rm -f example2
	rm -f example3
	rm -f example4
	rm -f example5
	rm -f example6
	rm -f example7
	rm -f example8
	rm -f example9
	rm -f example10
	rm -f depthrender
	rm -f gimbal-lock
	rm -f postprocess
	rm -f discardfragments
	rm -rf *.dSYM
