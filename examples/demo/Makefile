demo:	demo.cpp ../../src/LuaBinding.h MyActor.h MyActorBinding.h MyVector3d.h MyVector3dBinding.h Makefile
	g++ -O3 -g ../common/common.cpp demo.cpp -I../../src -I../common -o demo -llua

clean:
	-rm -f demo
