SUBDIRS = cmd_parser
INCLUDES = $(XMMS2_CFLAGS) -I@top_srcdir@/
LIBS = $(XMMS2_LIBS) $(READLINE_LIBS)
LDADD = cmd_parser/libcmd_parser.a
bin_PROGRAMS = nyello
nyello_SOURCES = nyello.hh nyello.cc\
		 dispatcher.hh dispatcher.cc\
		 readlinelistener.hh readlinelistener.cc
#		dispatcher.cc\
#		playback.cc\
#		medialibrary.cc\
#		output.cc\
#		columniser.cc\
#		column.cc\
#		countercolumn.cc\
#		printable.cc\
#		resultlist.cc\
#		playlistresultlist.cc\
#		selectionresultlist.cc\
#		abstractresult.cc\
#		songresult.cc\
#		selectresult.cc\
#		richresult.cc\
#		patternparser.cc\
#		patternnodes.cc\
#		patternquery.cc\
#		medialibquery.cc\
#		sequence.cc\
#		command.cc\
#		delayed.cc\
#		asynchronizer.cc\
#		productmaker.cc\
#		uri.cc
