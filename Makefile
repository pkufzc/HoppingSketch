objs = main.o bloom_filter.o hash.o hopping.o utils.o spec_cm.o env.o countsk.o cmsketch.o cusketch.o
CXXFLAGS = -O2 -std=c++11

main: $(objs)
	g++ -O2 -o main $(objs) 

.PHONY : clean
clean:
	rm main $(objs)