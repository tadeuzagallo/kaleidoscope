#include <cstdlib>
#include <string>

enum Token {
  tok_eof = -1,
  tok_def = -2, tok_extern = -3,
  tok_identifier = -4, tok_number = -5
};

static std::string IdentifierStr;
static double NumVal;

static int gettok() {
  static int lastChar = ' ';

  while(isspace(lastChar)) {
    lastChar = getchar();
  }

  if (isalpha(lastChar)) {
    IdentifierStr = lastChar;
    while (isalnum(lastChar = getchar())) {
      IdentifierStr += lastChar;
    }

    if (IdentifierStr == "def") return tok_def;
    else if (IdentifierStr == "extern") return tok_extern;
    else return tok_identifier;
  }

  if (isdigit(lastChar)) {
    std::string numStr;

    do {
      numStr += lastChar;
      lastChar = getchar();
    } while (isdigit(lastChar) || lastChar == '.');

    NumVal = strtod(numStr.c_str(), 0);
    return tok_number;
  }

  if (lastChar == '#') {
    do lastChar = getchar();
    while (lastChar != EOF && lastChar != '\n' && lastChar != '\r');
  }

  if (lastChar == EOF) {
    return tok_eof;
  }

  int thisChar = lastChar;
  lastChar = getchar();
  return thisChar;
}
