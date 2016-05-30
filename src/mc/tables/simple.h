typedef struct EdgeList {
  unsigned int edges[12];
} EdgeList;

EdgeList edgeTable[256] = {
  { .edges = { 0, 3, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1 } },
};
