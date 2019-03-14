//JaviTFG.ino
/*
	Copyright: Francisco Alarcón Sánchez.
	GitHub User: isacleibe
	Twitter: @franalasan
*/
//====================================================================================
#define PREGUNTA 0
#define ERROR 1
#define DIRECTO 2
#define INDIRECTO 3

const unsigned long PERMAQUINA = 50;

const unsigned long TIEMPO1 = 10000;	//10seg
const unsigned long TIEMPOESPERA = 20000;	//20seg

//====================================================================================
void Descarga(){
	Serial.println("BOOM");
}
//====================================================================================
struct Maquina{
	int estado;
	int estadoB;
	unsigned long cambioms;
	unsigned long perms;
	unsigned long lastms;
	//Necessary mark variables.
	int p5;
	int n5;
	int f5;

	int p8;
	int n8;
	int f8;

	int p11;
	int n11;
	int f11;

	int p14;
	int n14;
	int f14;

	int p17;
	int n17;
	int f17;
	
};
//Selecciona el tiempo asociado a esa descarga.
unsigned long SelTiempo(struct Maquina& m){
	unsigned long tms;
	R = random(m.p5, m.f17-1);
	if(R<=m.p5 && R>m.f5){
	    tms = 5000;
	    m.n5 += -1;
	}
	if(R<=m.p8 && R>m.f8){
	    tms = 8000;
	    m.n8 += -1;
	}
	if(R<=m.p11 && R>m.f11){
	    tms = 11000;
	    m.n11 += -1;
	}
	if(R<=m.p14 && R>m.f14){
	    tms = 14000;
	    m.n14 += -1;
	}
	if(R<=m.p17 && R>m.f17){
	    tms = 17000;
	    m.n17 += -1;
	}
	return tms;
}
//Realiza un ajuste de los valores de posicionamiento de las marcas temporales.
void Reajuste(struct Maquina& m){
	//m.p5 = 0;
	m.f5 = m.p5 + m.n5;

	m.p8 = m.f5;
	m.f8 = m.p8 + m.n8;

	m.p11 = m.f8;
	m.f11 = m.p11 + m.n11;

	m.p14 = m.f11;
	m.f14 = m.p14 + m.n14;

	m.p17 = m.f14;
	m.f17 = m.p17 + m.n17;
}
void setup_Maquina(struct Maquina& m, const unsigned long perms, const unsigned long currms){
	m.estado = 0;
	m.estadoB = 0;
	m.perms = perms;
	m.lastms = currms - m.perms;
	m.cambioms = currms;
	//Temporary mark variables.
	m.p5 = 0;
	m.n5 = 8;
	m.f5 = m.p5 + m.n5;

	m.p8 = m.f5;
	m.n8 = 4;
	m.f8 = m.p8 + m.n8;

	m.p11 = m.f8;
	m.n11 = 2;
	m.f11 = m.p11 + m.n11;

	m.p14 = m.f11;
	m.n14 = 1;
	m.f14 = m.p14 + m.n14;

	m.p17 = m.f14;
	m.n17 = 1;
	m.f17 = m.p17 + m.n17;	
}
void loop_Maquina(struct Maquina& m, const unsigned long currms){
	if(currms - m.lastms  >= m.perms){
		m.lastms += m.perms;
		m.estadoB = m.estado;

		Reajuste(m);	//Reajustamos las variables de tiempo.
		unsigned long tms = SelTiempo(m);	//Selecciona el tiempo.

		switch (m.estado) {
		    case PREGUNTA:		      
		      if(Serial.available()){
		          char C = Serial.Read();	//Lee caracter del puerto serial.
		          if(C == 'D'){
		              m.estado = DIRECTO;
		          } else {m.estado = INDIRECTO;}
		      }else if(currms - m.cambioms >= TIEMPO1){	//Si tarda mucho en elegir pasamos al estado de error
		          m.estado = ERROR;
		      }
		      break;

		    case ERROR:
		      Serial.println("ERROR");
		      break;

		    case DIRECTO:
		      int R = random(0, 1);	//Para que exista un 50% de probabilidad de descarga (0,1).
		      if(R){
		          Descarga();
		      }
		      break;

		    case INDIRECTO:		      
		      int R = random(1, 100);	//Posibilidad de descarga.
		      if(R < 15 && currms - m.cambioms >= tms){
		          Descarga();
		      }
		      //Pasa al siguiente estado cuando pasa el tiempo de espera, independientemente a si se realiza la descarga o no.
		      if(currms - m.cambioms >= TIEMPOESPERA){
		          m.estado = PREGUNTA
		      }
		      break;
		}

		if(m.estado != m.estadoB){
		    m.cambioms = currms;
		}
	}
}
//====================================================================================
struct Maquina m;

void setup() {
  unsigned long currms = millis();  
  setup_Maquina(m,PERMAQUINA, currms);
}

void loop() {
  unsigned long currms = millis();
  loop_Maquina(m, currms);
}


/*
	Uso de popms :	Cada vez que entramos a una secuencia determinada por el tiempo guarda la marca
					de tiempo de la entrada.
*/