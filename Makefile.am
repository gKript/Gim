## Process this file with automake to produce Makefile.in
## Created by Anjuta

SUBDIRS = src \
	include\
	gim_gkdp\
	gim_gkfp \
	gim_shell \
	documentation \
	gim_stat \
	gkmake \
	gimclassgen 

EXTRA_DIST = $(gimdoc_DATA) \
	$(documentation_DATA)

# Remove doc directory on uninstall
uninstall-local:
	-rm -r $(gimdocdir)

