main:	main.cpp Binding.h MyActor.h MyActorBinding.h MyVector3d.h MyVector3dBinding.h
	g++ -std=c++14 -O0 -g main.cpp -o main -llua

clean:
	rm main
