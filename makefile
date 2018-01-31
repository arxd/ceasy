.DEFAULT_GOAL := all

.PHONY: all clean pixie mei paint

pixie:
	make -C pixie

mei:
	make -C mei

paint:
	make -C paint

all:	pixie mei paint

clean:
	make -C pixie/ clean
	make -C paint/ clean
	make -C mei/ clean
	
