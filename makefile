EGCC=egcc/
EGVM=egvm/

execs: bin/
	make -C $(EGCC)
	mv $(EGCC)out/egcc bin/egcc
	make -C $(EGVM)
	mv $(EGVM)out/egvm bin/egvm

bin/:
	mkdir bin

clean:
	make -C $(EGCC) clean
	make -C $(EGVM) clean

purge: clean
	make -C $(EGCC) purge
	make -C $(EGVM) purge
	rm -rf bin/*

.PHONY: clean purge execs