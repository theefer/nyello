#include "command.hh"

Command::Command(char* _name, char* _shortName, DispFnPtr _action,
                 char* _usage, char* _description, char* _help)
  : action(_action), usage(_usage), description(_description), help(_help) {
  aliases.push_back(_name);
  aliases.push_back(_shortName);
}


Command::~Command() {

}


list<Command*>
Command::listAll() {
  list<Command*> cmds;
  Command* current;

  // Playback
  current = new Command("play", "p+", &Dispatcher::actionPlay,
                        "play",
                        "Start playback.",
                        "Start playback.\n");
  cmds.push_back(current);

  current = new Command("pause", "p-", &Dispatcher::actionPause,
                        "pause",
                        "Pause playback.",
                        "Pause playback.\n");
  cmds.push_back(current);

  current = new Command("toggle-play", "p", &Dispatcher::actionTogglePlay,
                        "toggle-play",
                        "Toggle playback (play/pause).",
                        "Toggle playback (play/pause).\n");
  cmds.push_back(current);

  current = new Command("stop", "s", &Dispatcher::actionStop,
                        "stop",
                        "Stop playback.",
                        "Stop playback.\n");
  cmds.push_back(current);

  current = new Command("next", "n", &Dispatcher::actionNext,
                        "next [OFFSET]",
                        "Jump forward in the playlist.",
                        "Jump a certain number of songs forward.\n"
                        "\n"
                        "If OFFSET is omitted, it defaults to 1 (next song).\n");
  cmds.push_back(current);

  current = new Command("previous", "r", &Dispatcher::actionPrevious,
                        "previous [OFFSET]",
                        "Jump backward in the playlist.",
                        "Jump a certain number of songs backward.\n"
                        "\n"
                        "If OFFSET is omitted, it defaults to 1 (previous song).\n");
  cmds.push_back(current);

  current = new Command("jump", "j", &Dispatcher::actionJump,
                        "jump POSITION|OFFSET",
                        "Jump to some position in the playlist or from a relative offset.",
                        "Jump to some position in the playlist or from a relative offset.\n"
                        "\n"
                        "If the argument is a simple number, jump to that position.  If it is\n"
                        "a number prefixed by + or -, jump of that offset from the current position.\n");
  cmds.push_back(current);

  // Search
  current = new Command("info", "i", &Dispatcher::actionInfo,
                        "info PATTERN",
                        "Find all songs matching a pattern.",
                        "Find all the songs matching the pattern.\n"
                        "\n"
                        "Patterns are expressions used to select groups of songs from the\n"
                        "library.  They are used in several commands that need to select songs\n"
                        "(enqueue, info, etc).  The syntax is quite rich, so you might need to\n"
                        "come back to the pattern documentation a few times.\n"
                        "\n"
                        "Patterns are constituted of conditions grouped by operators.\n"
                        "Conditions let you select groups of songs through different ways\n"
                        "(metadata filters, medialib ids, position in playlists, previous\n"
                        "results, etc).  Operators let you combine groups of songs to form new\n"
                        "groups.\n"
                        "\n"
                        "The following documentation describes the different types of\n"
                        "conditions as well as the operators used to combine them.\n"
                        "\n"
                        "\n"
                        "Flags\n"
                        "\n"
                        "  To query the song information from the server, flags must be used to\n"
                        "  choose which field to query:\n"
                        "\n"
                        "    -a, --artist <artist>\n"
                        "    -l, --album  <album>\n"
                        "    -t, --title  <title>\n"
                        "    -y, --year   <year>\n"
                        "    -g, --genre  <genre>\n"
                        "    -n, --track  <track>\n"
                        "    -z, --any    <any>\n"
                        "\n"
                        "  Examples:  -A Air          All songs by Air (exact artist match).\n"
                        "             --album meddle  All albums containing \"meddle\" in its name.\n"
                        "             Idioteque       All songs with \"idioteque\" in any of their infos.\n"
                        "\n"
                        "  Notes:\n"
                        "  - By default, values are case-insensitive and allow partial match.\n"
                        "  - If a flag is in uppercase, only exact matches of the value are kept.\n"
                        "  - When a value is given without a flag, the 'any' flag is assumed.\n"
                        "  - With short flags, several fields can be used in a flag:\n"
                        "      '-lG Soundtrack'  is equivalent to  '-l Soundtrack OR -G Soundtrack'\n"
                        "\n"
                        "\n"
                        "Medialib Id Sequences\n"
                        "\n"
                        "  To select songs using their medialib ids, simply specify a sequence of ids:\n"
                        "\n"
                        "    <sequence>\n"
                        "\n"
                        "  where the 'sequence' is a comma-separated list of ids or of ranges.\n"
                        "  Ranges are pairs of ids separated by a dash.  Bound ids are included\n"
                        "  and semi-bounded ranges are allowed.\n"
                        "\n"
                        "  Examples:  42              The song with id 42.\n"
                        "             13,22-25        Songs with id 13 or between 22 and 25 (included).\n"
                        "             777-            All songs with id higher or equal to 777.\n"
                        "\n"
                        "\n"
                        "Playlist Sequences\n"
                        "\n"
                        "  To select songs from an existing playlist, you can use playlist sequences:\n"
                        "\n"
                        "    <playlist>/<sequence>\n"
                        "\n"
                        "  where 'playlist' is the name of a playlist and 'sequence' is the\n"
                        "  position in that playlist, using the same syntax as describe above.\n"
                        "\n"
                        "  Examples:  MyParty/13,17   Songs 13 and 17 of the MyParty playlist.\n"
                        "             /-10            Ten first songs of the current playlist.\n"
                        "             /               All songs of the current playlist.\n"
                        "\n"
                        "  Notes:\n"
                        "  - The playlist defaults to the current playlist if omitted.\n"
                        "  - If the sequence is omitted, the whole playlist is selected.\n"
                        "\n"
                        "\n"
                        "History Sequences\n"
                        "\n"
                        "  To select songs from previous results, you can use history sequences:\n"
                        "\n"
                        "    N#<sequence>\n"
                        "\n"
                        "  where N is the index in the history stack of results (1=latest,\n"
                        "  2=second latest, etc) and 'sequence' is the position in that result\n"
                        "  set, using the usual sequence syntax.\n"
                        "\n"
                        "  Examples:  7#10-20         Songs 10 to 20 of the 7-th latest result set.\n"
                        "             3#              All songs of the 3-rd latest result set.\n"
                        "             #               All songs of the latest result set.\n"
                        "\n"
                        "  Notes:\n"
                        "  - N defaults to 1 if omitted (i.e. the latest result set).\n"
                        "  - If the sequence is omitted, the whole result set is selected.\n"
                        "  - Valid values of N range from 1 to the result history size.\n"
                        "\n"
                        "\n"
                        "Pattern References\n"
                        "\n"
                        "  To reuse a previous pattern, you can use pattern references:\n"
                        "\n"
                        "    +N\n"
                        "\n"
                        "  where N is the index in the history stack of patterns (1=latest,\n"
                        "  2=second latest, etc).\n"
                        "\n"
                        "  Examples:  4+              The 4-th latest pattern.\n"
                        "             +               The latest pattern.\n"
                        "\n"
                        "  Notes:\n"
                        "  - N defaults to 1 if omitted (i.e. the latest pattern).\n"
                        "  - Expressions '#' and '+' are NOT equivalent, as the former selects\n"
                        "    the latest results whereas the latter reuses the latest pattern.\n"
                        "    In case the medialib was changed, '+' will provide updated results.\n"
                        "\n"
                        "\n"
                        "Operators\n"
                        "\n"
                        "  To combine any kind of condition, there exist different operators:\n"
                        "\n"
                        "    &, AND\n"
                        "    |, OR\n"
                        "    %, XOR\n"
                        "    ^, NOT\n"
                        "\n"
                        "  Examples:  -A Beck AND -l odelay AND ( Jack-Ass OR Pollution )\n"
                        "             + OR Funk/-3,19 OR 8-10\n"
                        "             Goldfrapp mountain\n"
                        "\n"
                        "  Notes:\n"
                        "  - When no operator is used to separate conditions, AND is assumed.\n"
                        "  - Groups can be formed using parentheses (note: surround with spaces!)\n"
                        "  - Only one type of operator can be used in a single group; to use more,\n"
                        "    form more groups.\n"
                        "\n"
                        "\n"
                        "Ordering\n"
                        "\n"
                        "  The list of matching songs can be ordered by a custom list of fields\n"
                        "  using the order flag:\n"
                        "\n"
                        "    -o, --orderby  <fieldlist>\n"
                        "\n"
                        "  where fieldlist is a comma-separated list of fields (see the flags)\n"
                        "  to order by, either in short or long form.  Reverse ordering is done\n"
                        "  by specifying the field in uppercase.  When no ordering flag is\n"
                        "  specified, the default order sequence of nyello is used.\n");


  cmds.push_back(current);

  current = new Command("list", "l", &Dispatcher::actionList,
                        "list [PLAYLIST]",
                        "List the songs from a playlist.",
                        "List all the songs in the given playlist (the current playlist by default),\n"
                        "marking the playing song.\n");
  cmds.push_back(current);

  // Playlist edition
  current = new Command("enqueue", "e", &Dispatcher::actionEnqueue,
                        "enqueue PATTERN",
                        "Enqueue all songs matching a pattern.",
                        "Enqueue all the songs matching the pattern at the end of the playlist.\n"
                        "\n"
                        "See 'help info' for the syntax of patterns.\n");
  cmds.push_back(current);

  current = new Command("insert", "e+", &Dispatcher::actionInsert,
                        "insert PATTERN",
                        "Insert all songs matching a pattern after current song.",
                        "Insert all the songs matching the pattern after the current song.\n"
                        "\n"
                        "See 'help info' for the syntax of patterns.\n");
  cmds.push_back(current);

  current = new Command("replace", "e-", &Dispatcher::actionReplace,
                        "replace PATTERN",
                        "Replace the current song with all songs matching a pattern.",
                        "Replace the current song with all the songs matching the pattern.\n"
                        "\n"
                        "See 'help info' for the syntax of patterns.\n");
  cmds.push_back(current);

  current = new Command("clear", "c", &Dispatcher::actionClear,
                        "clear",
                        "Clear the current playlist.",
                        "Clear the current playlist.\n");
  cmds.push_back(current);

  current = new Command("shuffle", "u", &Dispatcher::actionShuffle,
                        "shuffle",
                        "Shuffle the current playlist.",
                        "Shuffle the current playlist.\n");
  cmds.push_back(current);

  // Playlists
  current = new Command("playlist-list", "pl", &Dispatcher::actionPlaylistList,
                        "playlist-list",
                        "List all the existing playlists.",
                        "List all the existing playlists, marking the active one.\n");
  cmds.push_back(current);

  current = new Command("playlist-use", "pu", &Dispatcher::actionPlaylistUse,
                        "playlist-use PLAYLIST",
                        "Change the active playlist.",
                        "Change the active playlist to the given playlist.\n");
  cmds.push_back(current);

  current = new Command("playlist-save-as", "ps", &Dispatcher::actionPlaylistSaveAs,
                        "playlist-save-as NAME",
                        "Save the current playlist under a new name.",
                        "Save the current playlist under the given name.\n");
  cmds.push_back(current);

  current = new Command("playlist-remove", "pr", &Dispatcher::actionPlaylistRemove,
                        "playlist-remove PLAYLIST",
                        "Remove a playlist.",
                        "Remove the given playlist.\n");
  cmds.push_back(current);

  // Help
  current = new Command("help", "h", &Dispatcher::actionHelp,
                        "help [COMMAND]",
                        "Display the help for all or a specific command.",
                        "Display the summary of all commands, or the help for a specific command\n"
                        "if one is given in argument.\n");
  current->addAlias("?");
  cmds.push_back(current);

  // Quit
  current = new Command("quit", "q", &Dispatcher::actionExit,
                        "quit",
                        "Exit nyello.",
                        "When run in interactive mode, this command terminates nyello.\n"
                        "\n"
                        "Note that this does not stop the server, so music might still be playing\n"
                        "after you stop nyello.\n");
  current->addAlias("exit");
  cmds.push_back(current);


  return cmds;
}

