.PHONY: all clean

FLAGS = -std=c99 -pedantic -g -D_XOPEN_SOURCE=700

all: IntervalMap CopyStuff

test: all
	./IntervalMap
	
testic: test_icList
	./test_icList

clean:
	rm -f IntervalMap IntervalMap.o P5pgmReader.o
	
IntervalMap: IntervalMap.o P5pgmReader.o ICList.o
	gcc $(FLAGS) -o IntervalMap IntervalMap.o P5pgmReader.o ICList.o -lm

IntervalMap.o: IntervalMap.c P5pgmReader.h ICList.h
	gcc $(FLAGS) -c IntervalMap.c -lm

P5pgmReader.o: P5pgmReader.c P5pgmReader.h
	gcc $(FLAGS) -c P5pgmReader.c

ICList.o: ICList.c ICList.h
	gcc  $(FLAGS) -c ICList.c

test_icList: test_icList.o ICList.o
	gcc $(FLAGS) -o test_icList test_icList.o ICList.o  -lm

test_icList.o: test_icList.c ICList.h
	gcc  $(FLAGS) -c test_icList.c  -lm

CopyStuff:
	cp ~/Documents/Program/makefile ~/Documents/Program/IntervalMap.c ~/Documents/Program/IntervalMap.h ~/Documents/Program/400x1280.pgm ~/Documents/Program/P5pgmReader.c ~/Documents/Program/P5pgmReader.h /home/jens/D/Dropbox/Studium/Bachelorarbeit/Program
