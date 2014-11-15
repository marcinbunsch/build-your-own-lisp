// Possible lval types
enum {
  LVAL_NUM, // number
  LVAL_ERR  // error
};

// Possible lval error types
enum {
  LERR_DIV_ZERO,
  LERR_BAD_OP,
  LERR_BAD_NUM
};

// lval stands for Lisp Value
typedef struct {
  int type;
  long num;
  int err;
} lval;

lval lval_num(long x);
lval lval_err(int x);
void lval_print(lval v);
void lval_println(lval v);

