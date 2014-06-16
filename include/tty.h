#ifndef TTY_H
#define TTY_H

/* TODO borrar despues, codigo repetido!
 */
#define NUMROWS 24//disponibles para las consolas,no incluye barra superior
#define NUMCOLS 80


typedef struct{
    MsgQueue_t * key_mq;
    unsigned short buf[NUMROWS][NUMCOLS];
    void * data;
    unsigned cur_x, cur_y, cur_attr;
    unsigned scrolls;
    bool cursor_on;
} Tty;

#endif
