main:	main.cpp Binding.h MyActor.h MyActorBinding.h
	g++ -std=c++14 -O0 -g main.cpp -o main -llua

clean:
	rm main
