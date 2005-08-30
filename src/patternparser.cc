#include "patternparser.hh"


PatternParser::PatternParser(Playback* _playback, MediaLibrary* _medialib)
  : playback(_playback), medialib(_medialib) {
}


PatternParser::~PatternParser() {
}


PatternQuery*
PatternParser::registerNewPattern(char** _arguments, int _numArgs) {
  PatternNode* top;
  PatternQuery* newQuery;
  char* use_order;

  // Init member vars
  arguments = _arguments;
  numArgs = _numArgs;

  // Generate query object and push it in history
  top = parse();
  if(top != NULL) {
    use_order = (orderby == NULL) ? defaultOrderby : orderby;
    newQuery = new PatternQuery(top, use_order);
    history.push_front(newQuery);
    padHistory();
  }

  return newQuery;
}


/**
 * Move the argument pointer forward and return it.
 */
char*
PatternParser::nextArgument() {
  ++currIndex;
  currArg = arguments[currIndex];
  return currArg;
}


/**
 * Start the parsing of the arguments and return the built tree.
 */
PatternNode*
PatternParser::parse() {
  // Reset current position
  currIndex = 0;
  currArg = arguments[0];

  // No pattern: select currently playing song
  if(numArgs == 0) {
    IdSequence* seq = new IdSequence();
    seq->addValue(playback->getCurrentId());
    return new PatternMLibSequence(seq);
  }
  // Parse the arguments
  else {
    return parseGroup();
  }
}


/**
 * Parse the group pointed to by the current argument and return the
 * top of the resulting subtree.
 */
PatternNode*
PatternParser::parseGroup() {
  PatternNode* group;
  PatternNode* elem;
  PatternOperator* newOperator;
  PatternOperator* currOperator = NULL;
  PatternNodeList* operands = new PatternNodeList();

  while(currIndex < numArgs) {
    elem = NULL;
    newOperator = NULL;

    // FIXME: Parse "ORDERBY" (-o or --orderby) BEFORE match flags! or together?
    //        THINK! Operator? place in group? in tree? elsewhere?


    // New group: add it to the list
    if(parseGroupStart()) {
      elem = parseGroup();
    }

    // Done parsing this group, quit the loop
    else if(parseGroupEnd()) {
      break;
    }

    // If an operator is parsed, use it for the group
    else if((newOperator = parseOperator()) != NULL) {
      if(currOperator == NULL) {
        currOperator = newOperator;
      }
      else if(currOperator->getOperatorId() != newOperator->getOperatorId()) {
        cerr << "Error: different operators found in the same group!" << endl;
      }
    }

    // If a condition is parsed, keep it as an operand
    else if((elem = parseCondition()) != NULL) {
    }

    // Save the element if valid
    if(elem != NULL) {
      operands->push_back(elem);
    }
  }

  // Pass operands to the operator
  if(currOperator != NULL) {
    currOperator->setOperands(operands);
    group = currOperator;
  }
  // No operator in the group
  else {
    if(operands->size() == 0)
      group = NULL;
    else if(operands->size() == 1)
      group = operands->front();
    else {
      // FIXME: several operands but no operator: AND implied?
      currOperator = new PatternOperatorAnd();
      currOperator->setOperands(operands);
      group = currOperator;
    }
  }

  return group;
}


/**
 * Parse the current argument as an operator, return the corresponding
 * PatternOperator* if valid.
 */
PatternOperator*
PatternParser::parseOperator() {
  PatternOperator* op = NULL;

  if((strcmp(currArg, "AND") == 0) || (strcmp(currArg, "&") == 0)) {
    op = new PatternOperatorAnd();
  }
  else if((strcmp(currArg, "OR") == 0) || (strcmp(currArg, "|") == 0)) {
    op = new PatternOperatorOr();
  }
  else if((strcmp(currArg, "XOR") == 0) || (strcmp(currArg, "%") == 0)) {
    op = new PatternOperatorXor();
  }
  else if((strcmp(currArg, "NOT") == 0) || (strcmp(currArg, "^") == 0)) {
    op = new PatternOperatorNot();
  }

  if(op != NULL) {
    nextArgument();
  }

  return op;
}


/**
 * Parse the current argument as a condition, return the corresponding
 * PatternNode* if valid.
 */
PatternNode*
PatternParser::parseCondition() {
  PatternNode* cond = NULL;

  // If starting by '+': reference to previous pattern
  if(*currArg == CHAR_PATTERNREF) {
    cond = parsePatternReference();
  }

  // If starting by '-': short or long match flag
  else if(*currArg == CHAR_MATCHFLAGS) {
    if(*(currArg+1) == CHAR_MATCHFLAGS)
      cond = parseLongMatchFlags();
    else
      cond = parseShortMatchFlags();
  }

  // If contains a '/': playlist position sequence
  else if(strrchr(currArg, '/') != NULL) {
    cond = parsePlaylistSequence();
  }

  // If contains a '#': pattern history sequence
  else if(strrchr(currArg, '#') != NULL) {
    cond = parseHistorySequence();
  }

  // If only composed of [\d,-]: medialib id sequence
  else if(strspn(currArg, "0123456789-,") == strlen(currArg)) {
    cond = parseMLibSequence();
  }

  // Unrecognized token, use it as an "any flag" value
  else {
    cond = new PatternMatchCondition(currArg);
  }


  if(cond != NULL) {
    nextArgument();
  }

  return cond;
}


/**
 * Determines whether the current argument starts a new group.
 */
bool
PatternParser::parseGroupStart() {
  bool retval = (strcmp(currArg, "(") == 0);
  if(retval)
    nextArgument();
  return retval;
}


/**
 * Determines whether the current argument end the current group.
 */
bool
PatternParser::parseGroupEnd() {
  bool retval = (strcmp(currArg, ")") == 0);
  if(retval)
    nextArgument();
  return retval;
}


PatternNode*
PatternParser::parseShortMatchFlags() {
  char* value;
  char* flags = currArg + 1;
  PatternNode* matchcond;

  // Get next token as unparsed value
  value = nextArgument();
  if(currIndex >= numArgs) {
    cerr << "Error: flag '" << flags << "' without value ignored!" << endl;
    return NULL;
  }

  if(strlen(flags) == 0) {
    cerr << "Error: an empty flag '-' and its value were ignored!" << endl;
    return NULL;
  }

  // Single flag
  if(strlen(flags) == 1) {
    matchcond = buildMatchCondition(*flags, value);
  }
  // Multiple flags, group them with an OR operator
  else {
    PatternNode* cond;
    PatternOperator* tmp_op;
    PatternNodeList* condlist = new PatternNodeList();
    for(; *flags != '\0'; ++flags) {
      cond = buildMatchCondition(*flags, value);
      if(cond != NULL) {
        condlist->push_back(cond);
      }
    }

    tmp_op = new PatternOperatorOr();
    tmp_op->setOperands(condlist);
    matchcond = tmp_op;
  }

  return matchcond;
}


PatternNode*
PatternParser::parseLongMatchFlags() {
  char* value;
  char* flags = currArg + 2;
  PatternNode* matchcond;

  // Get next token as unparsed value
  value = nextArgument();
  if(currIndex >= numArgs) {
    cerr << "Error: flag '" << flags << "' without value ignored!" << endl;
    return NULL;
  }

  if(strlen(flags) == 0) {
    cerr << "Error: an empty flag '-' and its value were ignored!" << endl;
    return NULL;
  }

  return buildMatchCondition(flags, value);
}


PatternNode*
PatternParser::parsePatternReference() {
  PatternNode* ref;
  char* nth = currArg + 1;
  unsigned int n;

  // Get id of referenced pattern (history indexes start at 0)
  n = (strlen(nth) == 0) ? 0 : charToId(nth) - 1;

  if(n >= 0 && n < history.size()) {
    ref = history[n]->getTopNode();
  }
  else {
    cerr << "Error: invalid pattern history reference '"
         << currArg << "'!" << endl;
    ref = NULL;
  }

  return ref;
}


/**
 * Parse the current argument as a sequence of position in a given
 * playlist, return the PatternPlaylistSequence* if the sequence is
 * valid, NULL otherwise.
 * Note that no check is done to make sure the playlist name is valid!
 */
PatternNode*
PatternParser::parsePlaylistSequence() {
  PatternNode* playlistSeq;
  char* pos     = strrchr(currArg, '/');
  char* seq_str = pos + 1;
  int name_len  = pos - currArg;
  char* plname;
  if(name_len > 0) {
    plname = new char[name_len + 1];
    strncpy(plname, currArg, name_len);
    plname[name_len] = '\0';
  }
  // No playlist name, use current playlist
  else {
    const char* buffer = medialib->getCurrentPlaylistName();
    plname = new char[ strlen(buffer) + 1 ];
    strcpy(plname, buffer);
  }

  IdSequence* seq = new IdSequence();
  if(!seq->parseAdd(seq_str)) {
    cerr << "Error: invalid playlist sequence '" << seq_str << "'!" << endl;
    playlistSeq = NULL;
  }
  else {
    playlistSeq = new PatternPlaylistSequence(plname, seq);
  }

  return playlistSeq;
}


PatternNode*
PatternParser::parseHistorySequence() {
  PatternNode* historySeq;
  char* seq_str = strrchr(currArg, '#') + 1;
  unsigned int n;

  IdSequence* seq = new IdSequence();
  if(!seq->parseAdd(seq_str)) {
    cerr << "Error: invalid history sequence '" << seq_str << "'!" << endl;
  }

  // Get id of referenced pattern (history indexes start at 0)
  n = (currArg == seq_str - 1) ? 0 : charToId(currArg) - 1;

  // Get the history sequence node
  if(n >= 0 && n < history.size()) {
    IdSequence* resSeq = history[n]->getIdsFromSequence(seq);
    historySeq = new PatternMLibSequence(resSeq);
  }
  else {
    cerr << "Error: invalid history reference '" << currArg << "'!" << endl;
    historySeq = NULL;
  }

  return historySeq;
}


/**
 * Parse the current argument as a sequence of mlib ids, return the
 * PatternMLibSequence* if the sequence is valid, NULL otherwise.
 */
PatternNode*
PatternParser::parseMLibSequence() {
  PatternNode* mlibSeq;
  IdSequence* seq = new IdSequence();
  if(!seq->parseAdd(currArg)) {
    cerr << "Error: invalid medialib sequence '" << currArg << "'!" << endl;
    mlibSeq = NULL;
  }
  else {
    mlibSeq = new PatternMLibSequence(seq);
  }

  return mlibSeq;
}


PatternCondition*
PatternParser::buildMatchCondition(char flag, char* value) {
  PatternCondition* cond;

  // FIXME: Code this by choosing class hierarchy
  switch(flag) {
  case 'a':  cond = new PatternMatchCondition("artist", value);        break;
  case 'A':  cond = new PatternMatchCondition("artist", value, true);  break;

  case 'l':  cond = new PatternMatchCondition("album", value);         break;
  case 'L':  cond = new PatternMatchCondition("album", value, true);   break;

  case 't':  cond = new PatternMatchCondition("title", value);         break;
  case 'T':  cond = new PatternMatchCondition("title", value, true);   break;

    /* FIXME: Special attributes
  case 'g':
  case 'y':
  case 'n':
    */

  case 'z':  cond = new PatternMatchCondition(value);        break;
  case 'Z':  cond = new PatternMatchCondition(value, true);  break;

  // FIXME: Special handling for orderby, save in member var!
  case 'o':  break;

  default:
    cerr << "Error: invalid short flag '" << flag << "' ignored!" << endl;
    cond = NULL;
    break;
  }

  return cond;
}

PatternCondition*
PatternParser::buildMatchCondition(char* flag, char* value) {
  PatternCondition* cond;

  // FIXME: comparing tolower(string) would be nicer

  if(strcmp(flag, "artist") == 0)
    cond = new PatternMatchCondition("artist", value);
  else if(strcmp(flag, "Artist") == 0 || strcmp(flag, "ARTIST") == 0)
    cond = new PatternMatchCondition("artist", value, true);

  else if(strcmp(flag, "album") == 0)
    cond = new PatternMatchCondition("album", value);
  else if(strcmp(flag, "Album") == 0 || strcmp(flag, "ALBUM") == 0)
    cond = new PatternMatchCondition("album", value, true);

  else if(strcmp(flag, "title") == 0)
    cond = new PatternMatchCondition("title", value);
  else if(strcmp(flag, "Title") == 0 || strcmp(flag, "TITLE") == 0)
    cond = new PatternMatchCondition("title", value, true);

  /* FIXME: special attributes
     genre, year, track
  */

  else if(strcmp(flag, "any") == 0)
    cond = new PatternMatchCondition(value);
  else if(strcmp(flag, "Any") == 0 || strcmp(flag, "ANY") == 0)
    cond = new PatternMatchCondition(value, true);

  // FIXME: Special handling for orderby, save in member var!
  else if(strcmp(flag, "orderby") == 0)
    cond = NULL;

  else {
    cerr << "Error: invalid long flag '" << flag << "' ignored!" << endl;
    cond = NULL;
  }

  return cond;
}


/**
 * Reduce the size of the history to fit MAX_HISTORY_LEN. Removed
 * PatternQuery objects are freed.
 */
void
PatternParser::padHistory() {
  PatternQuery* tmp;
  while(history.size() > MAX_HISTORY_LEN) {
    tmp = history.back();
    history.pop_back();
    delete tmp;
  }
}


/**
 * Convert a char* string into the integer number contained at the
 * beginning of that string.
 */
unsigned int
PatternParser::charToId(char* number) {
  char* end = number;
  int radix = 10;
  unsigned int value = strtol(number, &end, radix);

  // No character was parsed
  if(end == number) {
    // FIXME: NO! but how to return the error?
    return 0;
  }

  return value;
}
