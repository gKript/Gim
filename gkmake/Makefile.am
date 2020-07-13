## Process this file with automake to produce Makefile.in


bin_PROGRAMS =  \
	gkmake

gkmake_SOURCES =  \
	bmake_main.h \
	main.cc \
	bmake_header.cc

gkmake_CXXFLAGS = -O3 \
	-w \
	-L../src/ 

gkmake_LDADD = -lgim 

SUBDIRS = \
	gkm_data
