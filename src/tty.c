#include "tty.h"
/* TODO mover defines en comun como NUMROWS a un .h
 * y poner el include de ese .h acá
 */

void
tty_run(void *arg){
    shell_main(1, "shell", (Tty *)arg);
}

void 
initialize_tty(Tty *tty){
    int row, col;
    for(row = 0; row < NUMROWS; row++){
        for(col = 0; col < VIDBUF_SIZE; col++)
            (tty->buf)[row][col] = 0;
    }
    tty->data = NULL;
}

void
switch_focus(int tty_num){
    /* TODO: necesito un atomic() aca? */
    focus = tty[tty_num] 
    mt_reload_cons();
}
