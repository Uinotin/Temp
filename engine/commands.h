#ifndef COMMANDS_H
#define COMMANDS_H

#include "typedefs.h"

size_t GenBuffer(struct Rend *rend, char *data);
size_t BindBuffer(struct Rend *rend, char *data);
size_t LoadShader(struct Rend *rend, char *data);

#endif
