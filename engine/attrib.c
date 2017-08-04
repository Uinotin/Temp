#include "attrib.h"

void AttribPointersFunc(char **data)
{
  TempSizei i;
  TempSizei nAttribs;
  VertexAttrib *attribs;
  nAttribs = *((TempSizei *)(*data));
  *data += sizeof(TempSizei);
  attribs = (VertexAttrib *)(*data);
  *data += sizeof(VertexAttrib) * nAttribs;

  for (i = 0; i < nAttribs; ++i)
  {
    glVertexAttribPointer(attribs[i].index, attribs[i].attrib.size, attribs[i].attrib.type, GL_FALSE, attribs[i].stride, (const TempVoid *)((char *)0 + attribs[i].attrib.offset));
    glEnableVertexAttribArray(attribs[i].index);
  }
}
void AttribPointers(QueueData *queueData, TempSizei nAttribs, VertexAttrib *attribs)
{
  AppendParameter(queueData, &nAttribs, sizeof(TempSizei));
  AppendParameter(queueData, attribs, sizeof(VertexAttrib)*nAttribs);
  AppendCommand(queueData,&AttribPointersFunc);
}
