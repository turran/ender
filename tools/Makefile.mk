toolsdir = $(pkgdatadir)/tools
tools_DATA = \
tools/fromdoxygen.xslt \
tools/merge.xslt

EXTRA_DIST += $(tools_DATA)
