LIBS=-lGL -lglfw -lfreetype
INC=-I/usr/include/freetype2

run:
	g++ -std=c++11 -Wall -g assign3.cpp GlyphExtractor.cpp -o assign3 $(LIBS) $(INC)
	./assign3

clean:
	rm assign3
