#include "patternparser.hh"


PatternParser::PatternParser(Playback* _playback, MediaLibrary* _medialib)
  : playback(_playback), medialib(_medialib) {

  // FIXME: Default ordering shouldn't be hardcoded
  defaultOrderBy = new PatternOrderBy();
  defaultOrderBy->addOrder(new PatternOrderField("artist",  true));
  defaultOrderBy->addOrder(new PatternOrderField("album",   true));
  defaultOrderBy->addOrder(new PatternOrderField("tracknr", true));
  defaultOrderBy->addOrder(new PatternOrderField("url",     true));
}


PatternParser::~PatternParser() {
}


PatternQuery*
PatternParser::registerNewPattern(char** _arguments, int _numArgs) {
  PatternNode* top;
  PatternQuery* newQuery = NULL;

  // Init member vars
  arguments = _arguments;
  numArgs = _numArgs;
  orderby = NULL;

  // Generate query object and push it in history
  top = parse();
  if(top != NULL) {
    newQuery = new PatternQuery(top, getOrder());
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


PatternOrderBy*
PatternParser::getOrder() {
  PatternOrderBy* use_order;

  // FIXME: Use a copy of the default order, so we can free it!
  if(orderby == NULL)
    use_order = defaultOrderBy;
  else
    use_order = orderby;

  return use_order;
}

/**
 * Start the parsing of the arguments and return the built tree.
 */
PatternNode*
PatternParser::parse() {
  PatternNode* top;

  // Reset current position
  currIndex = 0;
  currArg = arguments[0];

  top = parseGroup();
  if((numArgs == 0) || (top == NULL && orderby != NULL)) {
    IdSequence* seq = new IdSequence();

    Delayed<unsigned int>* res = playback->getCurrentId();
    seq->addValue(res->getProduct());
    delete res;

    top = new PatternMLibSequence(seq);
  }

  return top;
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

    // New group: add it to the list
    if(parseGroupStart()) {
      nextArgument();
      elem = parseGroup();
    }

    // Done parsing this group, quit the loop
    else if(parseGroupEnd()) {
      nextArgument();
      break;
    }

    // If an operator is parsed, use it for the group
    else if((newOperator = parseOperator()) != NULL) {
      if(currOperator == NULL) {
        currOperator = newOperator;
      }
      else {
        if(currOperator->getOperatorId() != newOperator->getOperatorId()) {
          cerr << "Warning: different operators found in the same group, keeping first!" << endl;
        }
        delete newOperator;
      }
    }

    // If a condition is parsed, keep it as an operand
    else if((elem = parseCondition()) != NULL) {
    }

    // Save the element if valid
    if(elem != NULL) {
      operands->push_back(elem);
    }

    nextArgument();
  }

  // No operator in the group, default to AND
  if(currOperator == NULL) {
    currOperator = new PatternOperatorAnd();
  }

  // Pass operands to the operator
  if(operands->size() == 0) {
    group = NULL;
    delete currOperator;
    delete operands;
  }
  // FIXME: Would be better handled *in* setOperands()
  else if((operands->size() == 1)
          && (currOperator->getOperatorId() != PatternOperator::OPERATOR_NOT)) {
    group = operands->front();
    delete currOperator;
    delete operands;
  }
  else {
    currOperator->setOperands(operands);
    group = currOperator;
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

  // If only composed of [\d,-]: medialib id sequence
  else if(strspn(currArg, "0123456789-,") == strlen(currArg)) {
    cond = parseMLibSequence();
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

  // Unrecognized token, use it as an "any flag" value
  else {
    cond = new PatternMatchCondition(makeCopy(currArg));
  }

  return cond;
}


/**
 * Determines whether the current argument starts a new group.
 */
bool
PatternParser::parseGroupStart() {
  return (strcmp(currArg, "(") == 0);
}


/**
 * Determines whether the current argument end the current group.
 */
bool
PatternParser::parseGroupEnd() {
  return (strcmp(currArg, ")") == 0);
}


PatternNode*
PatternParser::parseShortMatchFlags() {
  char* value;
  char* flags = currArg + 1;
  PatternNode* matchcond;

  // Get next token as unparsed value
  value = nextArgument();
  if(currIndex >= numArgs) {
    cerr << "Warning: flag '" << flags << "' without value ignored!" << endl;
    return NULL;
  }

  if(strlen(flags) == 0) {
    cerr << "Warning: an empty flag '-' and its value were ignored!" << endl;
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
    plname = makeCopy(currArg, name_len);
  }
  // No playlist name, use current playlist
  else {
    // FIXME: Use makeCopy() despite the const ?
    const char* buffer = medialib->getCurrentPlaylistName().c_str();
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
    IdSequence* resSeq;
    if(!seq->empty()) {
      resSeq = history[n]->getIdsFromSequence(seq);
    }
    else {
      resSeq = history[n]->getIds();
    }
    historySeq = new PatternMLibSequence(resSeq);
  }
  else {
    cerr << "Error: invalid history reference '" << currArg << "'!" << endl;
    historySeq = NULL;
  }

  delete seq;

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


PatternOrderBy*
PatternParser::parseOrderBy(char* _orderstr) {
  PatternOrderBy* orderby = new PatternOrderBy();
  string orderstr = _orderstr;
  int offset, len, pos;
  string token;

  for(pos = 0, offset = 0; pos != string::npos; pos = orderstr.find(',', pos + 1), offset = pos + 1) {
    len = orderstr.find(',', offset);
    // FIXME: Substr with string::npos as arg2 ok?
    token = orderstr.substr(offset, len - offset);

    // FIXME: there must be a nicer way to do that switching?
    if(token.compare("a") == 0 || token.compare("artist") == 0)
      orderby->addOrder(new PatternOrderField("artist", true));
    else if(token.compare("l") == 0 || token.compare("album") == 0)
      orderby->addOrder(new PatternOrderField("album", true));
    else if(token.compare("t") == 0 || token.compare("title") == 0)
      orderby->addOrder(new PatternOrderField("title", true));
    else if(token.compare("n") == 0 || token.compare("tracknr") == 0)
      orderby->addOrder(new PatternOrderField("tracknr", true));
    else if(token.compare("g") == 0 || token.compare("genre") == 0)
      orderby->addOrder(new PatternOrderField("genre", true));
    else if(token.compare("y") == 0 || token.compare("year") == 0)
      orderby->addOrder(new PatternOrderField("year", true));

    // Special value: random
    else if(token.compare("r") == 0 || token.compare("random") == 0)
      orderby->addOrder(new PatternOrderFunction("RANDOM()"));

    if(token.compare("A") == 0
       || token.compare("Artist") == 0 || token.compare("ARTIST") == 0)
      orderby->addOrder(new PatternOrderField("artist", false));
    else if(token.compare("L") == 0
            || token.compare("Album") == 0 || token.compare("ALBUM") == 0)
      orderby->addOrder(new PatternOrderField("album", false));
    else if(token.compare("T") == 0
            || token.compare("Title") == 0 || token.compare("TITLE") == 0)
      orderby->addOrder(new PatternOrderField("title", false));
    else if(token.compare("N") == 0
            || token.compare("Tracknr") == 0 || token.compare("TRACKNR") == 0)
      orderby->addOrder(new PatternOrderField("tracknr", false));
    else if(token.compare("G") == 0
            || token.compare("Genre") == 0 || token.compare("GENRE") == 0)
      orderby->addOrder(new PatternOrderField("genre", false));
    else if(token.compare("Y") == 0
            || token.compare("Year") == 0 || token.compare("YEAR") == 0)
      orderby->addOrder(new PatternOrderField("year", false));
  }

  return orderby;
}


PatternCondition*
PatternParser::buildMatchCondition(char flag, char* value) {
  PatternCondition* cond;

  switch(flag) {
  case 'a':  cond = new PatternMatchCondition("artist", makeCopy(value));        break;
  case 'A':  cond = new PatternMatchCondition("artist", makeCopy(value), true);  break;

  case 'l':  cond = new PatternMatchCondition("album", makeCopy(value));         break;
  case 'L':  cond = new PatternMatchCondition("album", makeCopy(value), true);   break;

  case 't':  cond = new PatternMatchCondition("title", makeCopy(value));         break;
  case 'T':  cond = new PatternMatchCondition("title", makeCopy(value), true);   break;

    /* FIXME: Special attributes
  case 'g':
  case 'y':
  case 'n':
    */

  case 'z':  cond = new PatternMatchCondition(makeCopy(value));        break;
  case 'Z':  cond = new PatternMatchCondition(makeCopy(value), true);  break;

  // Special handling for orderby, save in member var!
  case 'o':
    orderby = parseOrderBy(value);
    cond = NULL;
    break;

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
    cond = new PatternMatchCondition("artist", makeCopy(value));
  else if(strcmp(flag, "Artist") == 0 || strcmp(flag, "ARTIST") == 0)
    cond = new PatternMatchCondition("artist", makeCopy(value), true);

  else if(strcmp(flag, "album") == 0)
    cond = new PatternMatchCondition("album", makeCopy(value));
  else if(strcmp(flag, "Album") == 0 || strcmp(flag, "ALBUM") == 0)
    cond = new PatternMatchCondition("album", makeCopy(value), true);

  else if(strcmp(flag, "title") == 0)
    cond = new PatternMatchCondition("title", makeCopy(value));
  else if(strcmp(flag, "Title") == 0 || strcmp(flag, "TITLE") == 0)
    cond = new PatternMatchCondition("title", makeCopy(value), true);

  /* FIXME: special attributes
     genre, year, track
  */

  else if(strcmp(flag, "any") == 0)
    cond = new PatternMatchCondition(makeCopy(value));
  else if(strcmp(flag, "Any") == 0 || strcmp(flag, "ANY") == 0)
    cond = new PatternMatchCondition(makeCopy(value), true);

  // Special handling for orderby, save in member var!
  else if(strcmp(flag, "orderby") == 0) {
    orderby = parseOrderBy(value);
    cond = NULL;
  }

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

char*
PatternParser::makeCopy(char* orig) {
  return makeCopy(orig, strlen(orig));
}

char*
PatternParser::makeCopy(char* orig, unsigned int length) {
  char* copy = new char[ length + 1 ];
  strncpy(copy, orig, length);
  copy[length] = '\0';
  return copy;
}
