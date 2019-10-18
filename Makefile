
.PHONY: src examples clean

all : 
	$(MAKE) src 
	$(MAKE) examples

src :
	$(MAKE) -C src

examples:
	$(MAKE) -C examples

clean:
	cd src; make clean
	cd examples; make clean
	rm -f *~
	rm -f include/*~
	rm -f data/shaders120/*~
	rm -f data/shaders460/*~

