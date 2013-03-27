
.PHONY: src examples clean

all : src examples

src :
	cd src; make

examples:
	cd examples; make

clean:
	cd src; make clean
	cd examples; make clean
	rm -f *~
