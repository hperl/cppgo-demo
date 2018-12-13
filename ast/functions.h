typedef struct {
  void *data;
  int64_t size;
} Buffer;

Buffer *findFunctions(char *code);
void Buffer_free(Buffer *);
