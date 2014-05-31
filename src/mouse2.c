#include <kernel.h>

// Definiciones del controlador de mouse PS/2
#define MOUSE				0x60
#define MOUSE_CTL			0x64
#define MOUSE_IBF			0x01
#define MOUSE_OBF			0x02
#define MOUSE_INT			12
#define MOUSE_ENABLE		0xA8
#define MOUSE_GETSTATUS		0x20
#define MOUSE_SETSTATUS		0x60
#define MOUSE_ENAIRQ		0x02
#define MOUSE_CMD			0xD4
#define MOUSE_DEFAULT		0xF6
#define MOUSE_ENASTREAM		0xF4
#define MOUSE_ACK			0xFA
#define MOUSE_NACK			0xFF

// Parámetros de recepción por polling
#define MOUSE_NTRIES		1000
#define MOUSE_DELAY			10


typedef unsigned char byte;
typedef unsigned int dword;



// Tarea de entrada del mouse
#define MOUSE_PRIO			10000		// Alta prioridad, para que funcione como "bottom half" de la interrupción
#define MOUSE_BUFSIZE		32
static byte mouse_cycle=0;     //unsigned char
unsigned char mouse_byte[3];    //signed char
static int veces=1;
static unsigned int mouse_x=0;         //signed char
static unsigned int mouse_y=0;         //signed char





// Interrupción de mouse.
static void 
mouse_int(unsigned irq)
{
	// Interrupción de mouse.
	//unsigned c = inb(MOUSE);
	//PutMsgQueueCond(scan_mq, &c);
	//printk("LLEGO interrupcion MOUSE \n");
	//veces++;

	 switch(mouse_cycle)
  {
    case 0:
      mouse_byte[0]=inb(0x60);
      mouse_cycle++;
      break;
    case 1:
      mouse_byte[1]=inb(0x60);
      mouse_cycle++;
      break;
    case 2:{
		      mouse_byte[2]=inb(0x60);
		      mouse_x=mouse_byte[1];
		      mouse_y=mouse_byte[2];
		      mouse_cycle=0;
		    
			      //delta y negativo
			  if((mouse_byte[0] & 0x20)==0x20){
			    mouse_y-=mouse_byte[2];
			   // printk("movY negativo\n");
			  }else{//deltaY positivo
			      mouse_y+=mouse_byte[2];
			     // printk("movY positivo\n");
			  }


			  //deltaX negativo
			  if((mouse_byte[0] & 0x10)==0x10){

			   unsigned int aux=mouse_byte[1];
			   if(mouse_x>0)
			    	mouse_x-=aux;

			    printk("x: %u \n",mouse_x);
			    //printk("mov x negativo\n");
			  }else{//deltaX positivo

			    //printk("mov x positivo\n");
			    	unsigned int aux=mouse_byte[1];
			      if(aux<NUMCOLS)//solo incrementa mientras no se pase de los limites de la pantalla
			      	mouse_x+=aux;
			      printk("desplazamiento positivo en %u unidades \n",aux);
			      printk("x: %u \n",mouse_x);
			  }
	  

	  //printk("X: %d Y: %d. \n", mouse_x, mouse_y);





      break;
  	}
  }



  
}

// Escribe en el registro de datos del controlador
static void 
mouse_send(unsigned data)
{
	//printk("mouse_send 0x%02.2x\n", data);
    while ( inb(MOUSE_CTL) & MOUSE_OBF )
		Yield();
    outb(MOUSE, data);
}

// Escribe en el registro de control del controlador
static void 
mouse_send_ctl(unsigned data)
{
	//printk("mouse_send_ctl 0x%02.2x\n", data);
    while ( inb(MOUSE_CTL) & MOUSE_OBF )
		Yield();
    outb(MOUSE_CTL, data);
}

// Lee del registro de datos del controlador
static void
mouse_receive(unsigned *p, bool wait)
{
	unsigned tries_left;

	for ( tries_left = MOUSE_NTRIES ; tries_left && !(inb(MOUSE_CTL) & MOUSE_IBF) ; tries_left-- )
		Yield();
	if ( wait && !tries_left )
	{
		//printk("mouse_receive delay\n");
		Delay(MOUSE_DELAY);
		if ( !(inb(MOUSE_CTL) & MOUSE_IBF) );
		//	printk("mouse_receive timeout\n");
	}
	unsigned char c = inb(MOUSE);
	if ( p )
		*p = c;
	//printk("mouse_receive 0x%02.2x\n", c);
}

// Esta inicialización se hace con interrupción de mouse inactiva.
// Las respuestas se leen por polling.
static void
init_mouse(void)
{
	// Habilitar PS2 auxiliar
	mouse_send_ctl(MOUSE_ENABLE);
	mouse_receive(NULL, false);		// ignoramos la respuesta

	// Habilitar generación de IRQ12 leyendo y modificando
	// el "compaq status byte"
	unsigned status;
	mouse_send_ctl(MOUSE_GETSTATUS);
	mouse_receive(&status, false);	// suponemos que no va a fracasar
	status |= MOUSE_ENAIRQ;			// habilitar la generación de IRQ12
	mouse_send_ctl(MOUSE_SETSTATUS);
	mouse_send(status);
	mouse_receive(NULL, false);		// ignoramos la respuesta

	// Setear parámetros default
	mouse_send_ctl(MOUSE_CMD);
	mouse_send(MOUSE_DEFAULT);
	mouse_receive(NULL, true);		// ignoramos la respuesta

	// Habilitar el mouse para que mande eventos
	mouse_send_ctl(MOUSE_CMD);
	mouse_send(MOUSE_ENASTREAM);
	mouse_receive(NULL, true);		// ignoramos la respuesta
	printk("posicion inicial x: %u",mouse_x);
}



// Interfaz pública

void
mt_mouse_init(void)
{
	
	init_mouse();
	mt_set_int_handler(MOUSE_INT, mouse_int);
	mt_enable_irq(MOUSE_INT);
}

