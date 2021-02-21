
#ifndef UTILS_C
#define UTILS_C

#include <string.h>
#include <stdbool.h>

/**Concat a bunch of strings together
 * 
 * count - number of strings in strs
 * strs - an array of strings
 * joiner - a string to insert in between each string in strs
 */
char * string_concat (char** strs, int start, int count, char* joiner) {
  //total length of chars in the result (not including null terminator)
  int totalLength = 0;

  //joiner info
  bool useJoiner = joiner != 0;
  int joinerLength;
  if (useJoiner) joinerLength = strlen(joiner);

  //reusable loop string
  char * current;
  //reusable loop string's length
  int currentLength = 0;
  //lengths for each string in strs
  int strslengths[count];

  //loop through strs
  for (int i=start; i<count; i++) {
    current = strs[i];
    //calculate the length
    currentLength = strlen(current);

    //copy into list of lengths
    strslengths[i] = currentLength;
    //add to total
    totalLength += currentLength;

    //if a joiner is specified, add it for each item except the last one
    if (useJoiner && i != count-1) totalLength += joinerLength;
  }

  //the resulting string
  //make sure to add one for the null terminator
  char * result = malloc(totalLength + 1);
  //marker for the loop when copying chars
  int offset = 0;

  //loop through strs again
  for (int i=start; i<count; i++) {
    current = strs[i]; //current string
    currentLength = strslengths[i]; //current length

    //loop through the current string
    for (int j=0; j<currentLength; j++) {
      //copy the contents into result in the right spot
      //offset marks the result starting offset where current will be copied to
      //j marks the individual character of the current string
      result[offset + j] = current[j];
    }
    offset += currentLength;
    
    //copy in joiner data as well (except for after last string)
    if (useJoiner && i != count-1) {
      for (int j=0; j<joinerLength; j++) {
        result[offset + j] = joiner[j];
      }
      offset += joinerLength;
    }
  }
  //arrays start at 0, so using totalLength as the index here gives the last index
  //which we reserved for a null terminator
  //lets explicitly set it to null ffs
  result[totalLength] = 0;

  //we're done
  return result;
}

#endif
