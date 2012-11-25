#ifndef PROCESS_H
#define PROCESS_H

#include "revision.h"

class Process
{
public:
    Process(Revision *r, int id);
    Revision *rev;
    int id;
};

#endif // PROCESS_H
