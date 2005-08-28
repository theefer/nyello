#-----Macros---------------------------------

BASEDIR=/usr/local/include/xmms2/

INCLUDES = -I. -I$(BASEDIR)

CXX = g++
CXXFLAGS = -g $(INCLUDES)

#-----Suffix Rules---------------------------
# set up C++ suffixes and relationship between .cc and .o files

.SUFFIXES: .cc

.cc.o:
	$(CXX) $(CXXFLAGS) -c $<

.cc :
	$(CXX) $(CXXFLAGS) -lreadline -lxmmsclient $< -o $@

#-----File Dependencies----------------------

SRC=nyello.cc\
	dispatcher.cc\
	playback.cc\
	medialibrary.cc\
	output.cc\
	columniser.cc\
	column.cc\
	countercolumn.cc\
	printable.cc\
	dictlist.cc\
	querysonglist.cc\
	playlistlist.cc\
	playlistsonglist.cc\
	patternparser.cc\
	patternnodes.cc\
	patternquery.cc\
	medialibquery.cc\
	sequence.cc

OBJ = $(addsuffix .o, $(basename $(SRC)))

nyello: $(OBJ)
	$(CXX) $(CXXFLAGS) -lreadline -lxmmsclient -o $@ $(OBJ)

#-----Other stuff----------------------------
depend:
	makedepend $(CXXFLAGS) -Y $(SRC)

clean:
	rm -f $(OBJ)
