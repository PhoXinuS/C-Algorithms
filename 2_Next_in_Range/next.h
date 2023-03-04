#ifndef _NEXT_H
#define _NEXT_H 1

#include <vector>

void init(const std::vector<int> &x);
int nextInRange(int i, int a, int b); // min{ j : j >= i && x[j] \in [a..b] } or -1
void done();

#endif
