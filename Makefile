PWD=$(shell pwd)
NAME=$(shell basename $(PWD))
IMAGE=$(NAME)
TAG=local
action?=test

DIRS := \
	array/spiral2d \
	divide-and-conquer/indexisvalue \
	dynamic-programming/staircase \
	graph/stepword \
	hash/patmatch \
	heap/topkmovies \
	random/randstream \
	random/urngfromflip \
	sort/mwaymergesort \
	string/anagrams

.PHONY: all $(DIRS)

all:: $(DIRS)

$(DIRS):
	# e.g. make action=(-insert-your-action-here-)
	$(MAKE) -C $@ $(action)

include Makefile.defs
