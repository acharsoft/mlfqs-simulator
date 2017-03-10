default: mlfqs.cpp
	g++ -std=c++0x -o mlfqs mlfqs.cpp

clean:
	rm -f mlfqs
