

#ifndef JSONLEXER_C
#define JSONLEXER_C

#include <stdbool.h>
#include "./lexer.c"

//--------string literal scanner

struct strlit_info {
  bool quote_seen;
  bool is_escaped;
};
char accept_stringliteral (int offset, char ch, void * info) {
  struct strlit_info * p = (struct strlit_info *) info;
  //don't allow lex if string didn't start with quote
  if (offset == 0 && ch != '"') {
    return accept_error;
  }

  if (ch == '"') {
    if (p->quote_seen) {
      return accept_done;
    } else {
      p->quote_seen = true;
      return accept_allow;
    }
  } else {
    return accept_allow;
  }
}

void scan_stringliteral (char* src, int start, struct scan_result * out) {

  struct strlit_info * info = malloc(sizeof(struct strlit_info));
  scan_string_for(src, start, info, accept_stringliteral, out);

  if (out->success) {
    //remove quotes from string after scan
    char * str_without_quotes = string_copy(out->value, 1, out->count-2);
    free(out->value); //properly get rid of old string
    out->value = str_without_quotes;

    free(info);
    return;
  }
}

//--------number literal scanner

struct numlit_info {
  bool digit_seen;
  bool decimal_seen;
  bool e_seen; //consider string "12e2"
  bool digit_follows_e; //a digit must follow "e"
};
char * DIGITS = "0123456789";
char accept_numberliteral (int offset, char ch, void * info) {
  struct numlit_info * p = (struct numlit_info *) info;

  bool debug = true;

  // if (debug) printf("[ %c ]", ch);

  if (char_in_string(DIGITS, ch)) {
    if (p->e_seen) p->digit_follows_e = true;
    p->digit_seen = true;
    return accept_allow;
  } else if (ch == '.') {
    //no double decimal allowed
    if (p->decimal_seen) {
      if (debug) printf("no multiple decimal allowed");
      return accept_error;
    } else {
      //decimal cannot follow e
      if (p->e_seen) {
        if (debug) printf("decimal can't follow e");
        return accept_error;
      }

      p->decimal_seen = true;
      return accept_allow;
    }
  } else if (ch == 'e') {
    //cannot begin with e
    if (offset == 0) return accept_error;
    
    //cannot have multiple of 'e'
    if (p->e_seen) {
      if (debug) printf("no multiple e allowed");
      return accept_error;
    }
    p->e_seen = true;
    return accept_allow;
  } else {
    //here ch is not a valid number char
    //so the best case is accept_terminator

    if (!p->digit_seen) {
      if (debug) printf("no digits in number");
      return accept_error;
    }

    //if e was seen but no digit after, not valid
    if (p->e_seen && !p->digit_follows_e) {
      if (debug) printf("e was seen, but no digit follows");
      return accept_error;
    }

    if (p->decimal_seen) {
      if (p->digit_seen) {
        return accept_terminator;
      } else {
        //a decimal without digits isn't valid
        if (debug) printf("decimal with no digits is invalid");
        return accept_error;
      }
    }

    return accept_terminator;
  }
}
void scan_numberliteral (char* src, int start, struct scan_result * out) {
  struct numlit_info * info = malloc(sizeof(struct numlit_info));
  info->decimal_seen = false;
  info->digit_follows_e = false;
  info->digit_seen = false;
  info->e_seen = false;
  scan_string_for(src, start, info, accept_numberliteral, out);
  free(info);
  return;
}

//--------white space scanner

char accept_whitespace (int offset, char ch, void * info) {
  if (ch == ' ' || ch == '\t' || ch == '\n') return accept_allow;
  return accept_terminator;
}
void scan_whitespace (char* src, int start, struct scan_result * out) {
  scan_string_for(src, start, 0, accept_whitespace, out);
  return;
}

#endif
