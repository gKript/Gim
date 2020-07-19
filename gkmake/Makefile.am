## Process this file with automake to produce Makefile.in


bin_PROGRAMS =  \
	gkmake

gkmake_SOURCES =  \
	main.cc \
	gkm_header.cc \
	gkm_main.h

gkmake_CXXFLAGS = -O3 \
	-w \
	-L../src/ 

gkmake_LDADD = -lgim 

SUBDIRS = \
	gkm_data
