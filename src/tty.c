#define VIDBUF_SIZE NUMCOLS*NUMROWS*2

struct{
    char buf[VIDBUF_SIZE];
    void * data;
    /* cola para lo del teclado */
} Tty;

void
tty_run(void *arg){
    shell_main(1, "shell", (Tty *)arg);
}

void 
initialize_tty(Tty *tty){
    int j;
    for(j = 0; j < VIDBUF_SIZE; j++)
        (tty->buf)[j] = 0;
    }
    tty->data = NULL;
}
