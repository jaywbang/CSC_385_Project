#ifndef PARSER_H
#define PARSER_H

#include "scanner.h"
#include "source.h"

struct node_s *parsesimplecommand(struct token1 *tok);

#endif
