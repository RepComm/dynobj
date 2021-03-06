
#ifndef LEXER_C
#define LEXER_C

#include "./boolean.h"
// #include <stdbool.h>

#define LexerScanResultP struct LexerScanResult *

bool char_in_string (char* haystack, char needle) {
  size_t ind = 0;
  char current = haystack[ind];
  while (current != 0) {
    if (current == needle) return true;
    ind ++;
    current = haystack[ind];
  }
  return false;
}

struct LexerScanResult {
  bool success; //4?
  size_t count; //4
  char * value; //4
};

struct LexerScanResult * scan_result_get (struct LexerScanResult * recycled) {
  if (recycled == 0) recycled = malloc(sizeof(struct LexerScanResult));

  recycled->count = 0;
  recycled->success = true;
  recycled->value = 0;

  return recycled;
}

char * string_copy (char * src, size_t start, size_t width) {
  char * result = malloc(width + 1); //remember null terminator
  strncpy(result, src+start, width);
  result[width] = 0;
  return result;
}

enum accept_answer {
  //when it doesn't accept the string because of bad format
  accept_error,
  //when the accept callback is ok with the char but wants to keep going
  accept_allow,
  //when it is satisfied and scan_result should be calculated/returned
  accept_done,
  //handles cases where its ok to allow_done
  //if a previous accept_allow was triggered, but not including the current char
  //consider string "0.001,"
  accept_terminator
};

/** Generic string scanner
  * You provide a simple `accept` callback function
  * which is ran over every char in `src` beginning at `start`
  * 
  * The callback function is provided a `persistence` struct
  * so it can remember states if desired (such as searching for more than one quote in a string literal)
  */
void scan_string_for (char * src, size_t start, void * persistence, char (*accept)( size_t offset, char ch, void * persistence ), struct LexerScanResult * out) {
  if (out == 0) return;

  //stores the result of scanning the string
  out->count = 0;
  out->success = false;
  out->value = 0;

  //the current char
  char ch = src[start];

  //a value returned by your accept callback function
  char accept_state = 0;

  bool accept_allow_seen = false;

  //loop through the string
  size_t srcMaxIndex = strlen(src)-1;
  size_t max = 1023;
  if (srcMaxIndex < max) max = srcMaxIndex;
  // printf("max index %li , max search acceptor %li\n", srcMaxIndex, max);

  for (size_t i=0; i<max; i++) {
    //grab the char
    ch = src[i+start];
    
    //test it against the accept callback
    accept_state = accept(i, ch, persistence);

    //handle what to do based on accept's answer
    if (accept_state == accept_allow) {
      //keep going
      accept_allow_seen = true;
    } else if (accept_state == accept_error) {
      //uh oh
      out->success = false;
      out->count = 0;
      out->value = 0;
      return;
    } else if (accept_state == accept_done) {
      //we're done
      out->success = true;
      out->count = i+1;
      out->value = string_copy(src, start, i+1);
      return;
    } else if (accept_state == accept_terminator) {
      //handle based on previous state
      if (accept_allow_seen) {
        //if any chars have been allowed previously, we're done
        out->success = true;
        out->count = i; //-1 relative to accept_done
        out->value = string_copy(src, start, i); //same here
      } else {
        //otherwise there were no valid chars read
        out->success = false;
        out->count = 0;
        out->value = 0;
      }
      return;
    }
  }
  
  //never reached accept_done or accept_terminator
  out->success = false;
  out->count = 0;
  out->value = 0;
  return;
}

#endif
