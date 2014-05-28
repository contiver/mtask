#define TTYS_NUM 7

Tty *tty[TTYS_NUM];
Tty *focus;

void 
mt_setup_ttys(void){
    int i, j;
    char buf[2];
    char name[] = {'t', 't', 'y', 0, 0}; 
    for(i = 0; i < TTYS_NUM; i++){
        tty[i] = malloc(sizeof Tty);
        initialize_tty(tty[i]);
        itao(i, buf, 10);
        Ready(CreateTask(tty_run, 0, NULL, tty[i], strcat(name, buf), DEFAULT_PRIO));
    }
    focus = *tty;
}
