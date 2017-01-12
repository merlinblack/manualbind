runtest:	runtest.cpp ../src/Binding.h MyActor.h MyActorBinding.h MyVector3d.h MyVector3dBinding.h
	g++ -std=c++14 -O0 -g runtest.cpp -I../src -o runtest -llua

clean:
	rm runtest
