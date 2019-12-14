PWD=$(shell pwd)
NAME=$(shell basename $(PWD))
IMAGE=$(NAME)
TAG=local
action?=test

DIRS := \
	arrays/spiral2d \
	heaps/topkmovies \
	random/urngfromflip \
	sorts/mwaymergesort

.PHONY: all $(DIRS)

all:: $(DIRS)

$(DIRS):
	# e.g. make action=(-insert-your-action-here-)
	$(MAKE) -C $@ $(action)

include Makefile.defs
