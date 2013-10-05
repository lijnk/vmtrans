#include "Parser.h"

char* _parser_stream;
char* _parser_com;
int _parser_cur;
int _parser_ln;
int _parser_sz;

void initParse(char* buf)
{
  _parser_stream = buf;
  _parser_com = NULL;
  _parser_cur = 0;
  _parser_sz = strlen(_parser_stream);
  _parser_ln = 0;
}

bool hasMoreCommands()
{
  if(_parser_cur < _parser_sz)
    return true;
  return false;
}

void resetCmd()
{
  if(_parser_com != NULL)
  {
    free(_parser_com);
    _parser_com = NULL;
  }
}

bool advance()
{
  //black magic here
  //commands will be parsed with "cmd "
  //this is to make argument finding easier
  //the whitespace found after the initial space will be removed
  //until a proper character is found.
  //all ends when the comment string is found or a newline
  //
  //also fuck crlf

  resetCmd();

  char* sp = _parser_stream+_parser_cur; //start pointer
  char* ep = strchr(sp, '\n'); //end pointer
  char* res; //result
  size_t size; //size of result

  if(ep == NULL) //check if there's actually a newline
    ep = strchr(sp, 0);

  _parser_cur = (ep - _parser_stream) + 1;

  char* tp = strstr(sp, "//"); //comments
  if(tp != NULL && tp < ep)
    ep = tp;

  size = ep-sp; //copy string
  res = (char*) calloc(size, sizeof(char));
  strncpy(res, sp, size);

  int ws = 0; //whitespace
  bool is = true; //if we kill space
  for(unsigned int i = 0; i<size; i++)
  {
    if(res[i] == ' ' || res[i] == '\t' || res[i] == '\r')
    {
      if(is)
      {
        ws++;
        res[i] = ' ';
      }
      else
        is = true;
    }
    else
      is = false;
  }

  _parser_com = (char*) calloc(size - ws + 1, sizeof(char));
  is = true;
  ws = 0;
  for(unsigned int i = 0; i<size; i++)
  {
    if(res[i] == ' ')
    {
      if(is)
        ws++;
      else
      {
        _parser_com[i-ws] = res[i];
        is = true;
      }
    }
    else
    {
      _parser_com[i-ws] = res[i];
      is = false;
    }
  }

  _parser_com[size-ws-1] = 0;
  _parser_ln++;
  free(res);

  if(_parser_com[0] == 0)
  {
    if(!hasMoreCommands())
      return false;

    resetCmd();
    return advance();
  }

  return true;
}

int commandType()
{
  char* tp = strchr(_parser_com, ' ');
  char* com;
  int res = -1;

  if((tp = strchr(_parser_com, ' ')) == NULL)
    tp = strchr(_parser_com, 0);

  const char* Type[] =
  {
    "add", "sub", "neg",
    "eq", "gt", "lt",
    "and", "or", "not",
    "push", "pop", "label",
    "goto", "if-goto", "function",
    "call", "return"
  };

  com = (char*) calloc(tp - _parser_com + 1, sizeof(char));
  strncpy(com, _parser_com, tp - _parser_com);
  com[tp - _parser_com] = 0;

  for(int i = 0; i<17; i++)
  {
    if(strcmp(com, Type[i]) == 0)
    {
      res = (i-8)<0 ? 0 : (i-8);
      break;
    }
  }

  free(com);

  return res;
}

char* arg1()
{
  char* sp = strchr(_parser_com, ' ');
  char* ep;
  char* res;

  if(commandType() == C_ARITHMETIC)
  {
    if(sp == NULL)
      sp = strchr(_parser_com, 0);

    res = (char*) calloc(sp-_parser_com+1, sizeof(char));
    strncpy(res, _parser_com, sp-_parser_com);
    res[sp-_parser_com] = 0;
    return res;
  }

  if(sp == NULL)
    return NULL;

  if((ep = strchr(sp+1, ' ')) == NULL)
    ep = strchr(sp, 0);

  res = (char*) calloc(ep-sp, sizeof(char));
  strncpy(res, sp+1, ep-sp-1);
  res[ep-sp-1] = 0;
  return res;
}

int arg2()
{
  char* sp = strrchr(_parser_com, ' ');
  char* ep;
  char* d;
  int res = -1;

  if(sp == NULL)
    return res;

  ep = strchr(sp, 0);

  d = (char*) calloc(ep-sp, sizeof(char));
  strncpy(d, sp+1, ep-sp);
  d[ep-sp] = 0;

  res = toNum(d);
  free(d);
  return res;
}

int toNum(char* s)
{
  int res = 0;
  int mul = 1;
  for(unsigned int i=strlen(s) - 1; i < strlen(s); i--)
  {
    if(s[i] < 48 || s[i] > 57)
      return -1;

    res += (s[i] -48) * mul;
    mul *= 10;
  }
  return res;
}

void debugString(char* s)
{
  printf("DEBUG: ");
  for(unsigned int i = 0; i<=strlen(s); i++)
  {
    printf("%d ", s[i]);
  }
  printf("\n");
}
