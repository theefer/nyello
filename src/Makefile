FILES=nyello.cc\
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
	playlistsonglist.cc

nyello: 
	g++ -I/usr/local/include/xmms2/ -lreadline -lxmmsclient -o nyello $(FILES)

test_columniser:
	g++ -o test_columniser test_columniser.cc columniser.cc column.cc printable.cc

clean:
	rm -f *~ *.o nyello
