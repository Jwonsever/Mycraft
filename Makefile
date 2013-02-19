CC = g++
ifeq ($(shell sw_vers 2>/dev/null | grep Mac | awk '{ print $$2}'),Mac)
	CFLAGS = -g -DGL_GLEXT_PROTOTYPES -I./include/ -I/usr/X11/include -DOSX
	LDFLAGS = -framework GLUT -framework OpenGL -L./lib/mac/ \
    	-L"/System/Library/Frameworks/OpenGL.framework/Libraries" \
    	-lGL -lGLU -lm -lstdc++ -lGLEW -lfreeimage 
else
	CFLAGS = -g -DGL_GLEXT_PROTOTYPES -I./include/ -I/usr/X11R6/include -I/sw/include \
					 -I/usr/sww/include -I/usr/sww/pkg/Mesa/include
	INCFLAGS = -I./glm-0.9.2.7 -I./include/ -I/usr/X11R6/include -I/sw/include \
		-I/usr/sww/include -I/usr/sww/pkg/Mesa/include
	LDFLAGS = -L./lib/nix -L/usr/X11R6/lib -L/sw/lib -L/usr/sww/lib \
						-L/usr/sww/bin -L/usr/sww/pkg/Mesa/lib -lglut -lGLU -lGL -lX11 -lGLEW -lfreeimage
endif
	
RM = /bin/rm -f 

all: main
main: main.o shaders.o shaders.h block.h raytrace.h variables.h Quaternion.h Quaternion.o
	$(CC) $(CFLAGS) -o main shaders.o main.o Quaternion.o $(INCFLAGS) $(LDFLAGS) 
main.o: main.cpp shaders.h block.h raytrace.h Quaternion.h variables.h sceneobject.h
	$(CC) $(CFLAGS) $(INCFLAGS) -c main.cpp
shaders.o: shaders.cpp
	$(CC) $(CFLAGS) $(INCFLAGS) -c shaders.cpp
Quaternion.o: Quaternion.cpp Quaternion.h
	$(CC) $(CFLAGS) $(INCFLAGS) -c Quaternion.cpp
clean: 
	$(RM) *.o main *.png