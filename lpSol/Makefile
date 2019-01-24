all:imm_lp


imm_lp: imm/*.cpp imm/*.h
	g++ -DDISCRETE imm/imm.cpp -w -Wall -std=c++0x -O3 imm/sfmt/SFMT.c  -o imm_lp

imm: imm/*.cpp imm/*.h
	g++ imm/imm.cpp -w -Wall -std=c++0x -O3 imm/sfmt/SFMT.c  -o imm

clean:
	rm imm_lp	
