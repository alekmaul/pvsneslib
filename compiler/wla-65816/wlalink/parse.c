
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "defines.h"



int get_next_token(char *in, char *out, int *pos) {

  int i, t;


  /* skip white space */
  for (i = 0; in[i] == ' ' || in[i] == 0x09; i++);
  if (in[i] == 0)
    return FAILED;

  for (t = 0; in[i] != ' ' && in[i] != 0x09 && in[i] != 0; i++, t++)
    out[t] = in[i];

  out[t] = 0;
  *pos += i;

  return SUCCEEDED;
}


int get_next_number(char *in, int *out, int *pos) {

  int i, o, n;


  /* skip white space */
  for (i = 0; in[i] == ' ' || in[i] == 0x09; i++);
  if (in[i] == 0)
    return FAILED;

  if (in[i] >= '0' && in[i] <= '9') {
    /* is it hex after all? */
    n = 0;
    for (o = 0; 1; o++) {
      if (in[i+o] >= '0' && in[i+o] <= '9')
				continue;
      if (in[i+o] >= 'a' && in[i+o] <= 'f') {
				n = 1;
				break;
      }
      if (in[i+o] >= 'A' && in[i+o] <= 'F') {
				n = 1;
				break;
      }
      if (in[i+o] == 'h' || in[i+o] == 'H') {
				n = 1;
				break;
      }
      break;
    }

    if (n == 1) {
      /* hex */
      o = 0;
      while (TRUE) {
				if (in[i] >= '0' && in[i] <= '9')
					o = (o * 16) + in[i] - '0';
				else if (in[i] >= 'a' && in[i] <= 'f')
					o = (o * 16) + in[i] - 'a' + 0xA;
				else if (in[i] >= 'A' && in[i] <= 'F')
					o = (o * 16) + in[i] - 'A' + 0xA;
				else if (in[i] == 'h' || in[i] == 'H')
					break;
				else if (in[i] == ' ' || in[i] == 0x09 || in[i] == 0)
					break;
				else
					return FAILED;
				i++;
      }
    }
    else {
      /* decimal */
      for (o = 0; in[i] >= '0' && in[i] <= '9'; i++)
				o = (o * 10) + in[i] - '0';
      if (!(in[i] == ' ' || in[i] == 0x09 || in[i] == 0))
				return FAILED;
    }
  }
  else if (in[i] == '$') {
    i++;
    o = 0;
    while (TRUE) {
      if (in[i] >= '0' && in[i] <= '9')
				o = (o * 16) + in[i] - '0';
      else if (in[i] >= 'a' && in[i] <= 'f')
				o = (o * 16) + in[i] - 'a' + 0xA;
      else if (in[i] >= 'A' && in[i] <= 'F')
				o = (o * 16) + in[i] - 'A' + 0xA;
      else if (in[i] == ' ' || in[i] == 0x09 || in[i] == 0)
				break;
      else
				return FAILED;
      i++;
    }
  }
  else
    return FAILED;

  *out = o;
  *pos += i;

  return SUCCEEDED;
}
