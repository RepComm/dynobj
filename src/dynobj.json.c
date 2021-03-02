
#ifndef DYNOBJ_JSON_C
#define DYNOBJ_JSON_C

#include "./dynobj.c"
#include "./jsonlexer.c"

#define JsonScanResultP struct JsonScanResult *

enum DynObj_JsonTypes {
  //A json parsed string (impl char * aka c-string)
  DynObj_Json_String,
  //A json parsed object (impl DynObjP)
  DynObj_Json_Object,
  //A json parsed number array (impl double[])
  DynObj_Json_NumberBuffer,
  //A json parsed number (impl double)
  DynObj_Json_Number
};

//Eventually this should be merged with LexerScanResult type
struct JsonScanResult {
  void * value;
  size_t count;
  bool success;
};

void expected_string (char * expected, char * got, size_t offset) {
  printf("Json read error, expected %s at offset %li, got %s\n", expected, offset, got);
}

void expected_char (char expected, char got, size_t offset) {
  if (got == 0) {
    printf("Json read error, expected '%c' at offset %li, ran into end of c-string\n", expected, offset);
  } else {
    printf("Json read error, expected '%c' at offset %li, got '%c'\n", expected, offset, got);
  }
}

/**Scan jsonString and try to generate a DynObj object
 * 
 * The result of scanning is saved to `result`, which you need to
 * allocate ahead of time
 * 
 * Ex:
 * `
 * char * src = "{}";
 * 
 * ...
 * 
 * struct JsonScanResult scan;
 * DynObj_scanObject (src, 0, &scan);
 * 
 * offset += scan.count;
 * `
 */
void DynObj_scanObject (char * jsonString, size_t start, JsonScanResultP result) {
  //set offset as a copy of start so we can refer to start later
  size_t offset = start;

  //The result from scanning the source text
  struct LexerScanResult scan;
  scan.success = false;
  scan.count = 0;
  scan.value = 0;
  
  result->count = 0;
  result->success = false;
  result->value = DynObj_create();

  //Remove extra whitespace
  scan_whitespace(jsonString, offset, &scan);
  offset += scan.count;

  //object begin
  if (jsonString[offset] != '{') {
    expected_char('{', jsonString[offset], offset);
    free(result->value);
    return;
  }
  printf("DynObj object:begin\n");
  offset++;

  bool objectEndedSafely = false;
  bool doObjectPropertyLoop = true;

  while (doObjectPropertyLoop) {
    //Remove extra whitespace
    scan_whitespace(jsonString, offset, &scan);
    offset += scan.count;

    //key
    scan_stringliteral(jsonString, offset, &scan);
    if (!scan.success) {
      printf("scan for key failed at offset %li aka '%c'\n", offset, jsonString[offset]);
      free(result->value);
      return;
    }
    offset += scan.count;

    char * key = scan.value;
    // printf("found key %s\n", key);

    //Remove extra whitespace
    scan_whitespace(jsonString, offset, &scan);
    offset += scan.count;

    //separator
    if (jsonString[offset] != ':') {
      expected_char(':', jsonString[offset], offset);
      free(result->value);
      return;
    }
    offset++;

    //Remove extra whitespace
    scan_whitespace(jsonString, offset, &scan);
    offset += scan.count;

    char predictor = jsonString[offset];
    if (predictor == '"') {
      scan_stringliteral(jsonString, offset, &scan);
      if (!scan.success) {
        //TODO error message here
        free(result->value);
        return;
      }
      offset += scan.count;

      //Copy the property to the result object
      ((DynObjP) result->value)->set(
        result->value,
        key,
        scan.value,
        DynObj_Json_String
      );

      printf("DynObj Json prop \"%s\" : \"%s\"\n", key, scan.value);

    } else if (predictor == '{') {
      struct JsonScanResult scanValueObj;
      DynObj_scanObject(jsonString, offset, &scanValueObj);
      if (!scanValueObj.success) {
        printf("DynObj error parsing child object at %li\n", offset);
        free(result->value);
        return;
      }

      //Copy the property to the result object
      ((DynObjP) result->value)->set(
        result->value,
        key,
        &scanValueObj,
        DynObj_Json_Object
      );
      printf("DynObj appended child\n");
      continue;
    } else if (predictor == '[') {
      //TODO
    } else if (char_in_string(DIGITS, predictor)) {
      scan_numberliteral(jsonString, offset, &scan);
      if (!scan.success) {
        printf("DynObj error parsing string value at %li\n", offset);
        free(result->value);
        return;
      }
      offset += scan.count;

      //Convert to double (our implementation of Json_Number)
      double * value = malloc(sizeof(double));
      sscanf(scan.value, "%lf", value);

      printf("DynObj Json prop \"%s\" : %lf\n", key, *value);

      //Copy the property to the result object
      ((DynObjP) result->value)->set(
        result->value,
        key,
        value,
        DynObj_Json_Number
      );
    } else {
      char got[] = {jsonString[offset], 0};
      expected_string("Object begin '{', array begin '[', or a digit 0-9", got, offset);
      free(result->value);
      return;
    }

    //Remove extra whitespace
    scan_whitespace(jsonString, offset, &scan);
    offset += scan.count;

    if (offset > strlen(jsonString)) {
      printf("offset was greater than src length at %li\n", offset);
      return;
    }

    predictor = jsonString[offset];
    if (predictor == ',') {
      // printf("DynObj next property\n");
      offset ++;
    } else if (predictor == '}') {
      printf("DynObj object:end with char at offset %li\n", offset);
      offset ++;
      objectEndedSafely = true;
      doObjectPropertyLoop = false;
      break;
    } else {
      //not going to lie I thought I'd have to write a function for this
      char ch[] = {predictor, 0};

      expected_string("property separator ',' or end of object '}'", ch, offset);
      free(result->value);
      return;
    }
  }
  if (!objectEndedSafely) {
    printf("DynObj Json object was not ended safely, freeing memory\n");
    free(result->value);
    return;
  }

  result->count = offset - start + 1;
  result->success = true;
  return; //explicit, optional, but whatever
}

DynObjP DynObj_fromJson (char * jsonString) {
  int offset = 0;
  struct JsonScanResult scan;
  DynObj_scanObject(jsonString, offset, &scan);

  return (DynObjP) scan.value;
}

#endif
