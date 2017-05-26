runtest:	runtest.cpp ../src/LuaBinding.h MyActor.h MyActorBinding.h MyVector3d.h MyVector3dBinding.h Makefile
	g++ -O3 -g runtest.cpp -I../src -o runtest -llua

clean:
	rm runtest
