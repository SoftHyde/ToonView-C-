# include <GL/glew.h>
# include <stdlib.h>
# include <cmath>
#ifdef __APPLE__
# include <OpenGL/gl.h>
# include <OpenGL/glu.h>
# include <GLUT/glut.h>
#else
# include <GL/gl.h>
# include <GL/glu.h>
# include <GL/glut.h>
#endif
#include <iostream>
#include <cmath>
using namespace std;

//Variables globales
int
	w=800,h=600, // tamaño de la ventana
	boton=-1, // boton del mouse clickeado
	lod=100, // nivel de detalle
	xclick,yclick; // x e y cuando clickeo un boton
bool 
	relleno=true,     // dibuja relleno o no
	rota=false,       // gira continuamente los objetos respecto de y
	wire=false,       // dibuja lineas o no
	dibuja=true,      // false si esta minimizado
	smooth=true,      // normales por nodo o por plano
	luz_fija=true,    // luz fija a la camara (o al mundo)
	blend=false,      // transparencias
	toon=true;
float
	lpos[]={2,1,5,0}, // posicion luz, l[4]: 0 => direccional -- 1 => posicional
	line_color[]={.2f,.3f,.4f,1},
	linewidth=1,pointsize=5, // ancho de lineas y puntos
	escala=150,escala0, // escala de los objetos window/modelo pixeles/unidad
	eye[]={0,0,5}, target[]={0,0,0}, up[]={0,1,0}, // camara, mirando hacia y vertical
	znear=2, zfar=20, //clipping planes cercano y alejado de la camara
	amy,amy0, // angulo del modelo alrededor del eje y
	ac0,rc0; // angulo resp x y distancia al target de la camara al clickear
	
static const double R2G=45/atan(1.0);
// temporizador:
static const int ms_lista[]={1,2,5,10,20,50,100,200,500,1000,2000,5000},ms_n=12;
static int ms_i=4,msecs=ms_lista[ms_i]; // milisegundos por frame

//A partir de aca declaro las cosas para el TP
GLuint ProgID;
//variables para el rotado del separador
float movimientoX=0,aux=0;
bool mov_plano=false;

void Motion_cb(int xm, int ym);
//A partir de aca comienzan las funciones

void drawTeapot(){
	glPushAttrib(GL_ALL_ATTRIB_BITS);
	//tetera
	glFrontFace(GL_CW);
	if (relleno){
		if (toon){
			glDisable(GL_LIGHTING);
			glDisable(GL_LIGHT0);
			glUseProgram(ProgID);
		}
		else {
			glUseProgram(0);
			glEnable(GL_LIGHTING);
			glEnable(GL_LIGHT0);
			glColor3f(0,1.f,0);
		}
		//glutSolidSphere(1,lod,lod);
		glutSolidTeapot(1);
	}
	if (wire){
		glUseProgram(0);
		glDisable(GL_LIGHTING);
		glColor4fv(line_color);
		//glutWireSphere(1,lod,lod);
		glutWireTeapot(1);
	}
	else{
		glUseProgram(ProgID);
	}
	glPopAttrib();
}
void buildStencil(){
	glPushMatrix();
		if(boton==GLUT_LEFT_BUTTON && mov_plano)
			aux=movimientoX;
		glRotatef(-amy,0,1,0);
		glTranslated(aux/50,0,0);
		
		glPushAttrib(GL_ALL_ATTRIB_BITS); // guardamos los atributos actuales
		glColorMask(false,false,false,false); // no dibuja en el color buffer
		glDisable(GL_DEPTH_TEST); // no actualiza el z buffer
		glDisable(GL_LIGHTING); // inhabilitamos el modelo de iluminacion
		
		//  @@@ Dibujar en el stencil los objetos necesarios.
		
		// STENCIL = 1 -> parte derecha
		glStencilFunc(GL_ALWAYS,1,~0);
		glStencilOp(GL_KEEP,GL_KEEP,GL_REPLACE); // Replace para setear el Stencil
		glBegin(GL_QUADS);
		glVertex3f(0,8,8);//A
		glVertex3f(18,8,8);//B
		glVertex3f(18,-8,8);//C
		glVertex3f(0,-8,8);//D
		
		glVertex3f(0,8,8);//A
		glVertex3f(18,8,8);//B
		glVertex3f(18,8,-8);//E	
		glVertex3f(0,8,-8);//F
		
		glVertex3f(0,8,-8);//A
		glVertex3f(18,8,-8);//B
		glVertex3f(18,-8,-8);//C
		glVertex3f(0,-8,-8);//D
		
		glVertex3f(0,-8,-8);//A
		glVertex3f(18,-8,-8);//B
		glVertex3f(18,-8,8);//E	
		glVertex3f(0,-8,8);//F
		glEnd();
		
		glStencilFunc(GL_ALWAYS,2,~0);
		glStencilOp(GL_KEEP,GL_KEEP,GL_REPLACE); // Replace para setear el Stencil
		glBegin(GL_QUADS);
		
		glVertex3f(0,8,8);//A
		glVertex3f(-18,8,8);//B
		glVertex3f(-18,-8,8);//C
		glVertex3f(0,-8,8);//D
		
		glVertex3f(0,8,8);//A
		glVertex3f(-18,8,8);//B
		glVertex3f(-18,8,-8);//E	
		glVertex3f(0,8,-8);//F
		
		glVertex3f(0,8,-8);//A
		glVertex3f(-18,8,-8);//B
		glVertex3f(-18,-8,-8);//C
		glVertex3f(0,-8,-8);//D
		
		glVertex3f(0,-8,-8);//A
		glVertex3f(-18,-8,-8);//B
		glVertex3f(-18,-8,8);//E	
		glVertex3f(0,-8,8);//F
		glEnd();
		
		glPopAttrib();
	glPopMatrix();
}
void separador(){
	glBegin(GL_QUADS);
	
	//planos paralelos al eje y
	glVertex3f(-.01,-1,1.2);
	glVertex3f(.01,-1,1.2);
	glVertex3f(.01,1,1.2);
	glVertex3f(-.01,1,1.2);
	
	glVertex3f(-.01,1,-1.2);
	glVertex3f(.01,1,-1.2);
	glVertex3f(.01,-1,-1.2);
	glVertex3f(-.01,-1,-1.2);
	
	//planos paralelos al eje -z
	glVertex3f(-.01,1,1.2);
	glVertex3f(.01,1,1.2);
	glVertex3f(.01,1,-1.2);
	glVertex3f(-.01,1,-1.2);
	
	glVertex3f(-.01,-1,-1.2);
	glVertex3f(.01,-1,-1.2);
	glVertex3f(.01,-1,1.2);
	glVertex3f(-.01,-1,1.2);
	glEnd();
}

void display_cb() {
	if (!dibuja) return;
	glClear(GL_DEPTH_BUFFER_BIT|GL_COLOR_BUFFER_BIT|GL_STENCIL_BUFFER_BIT);
	buildStencil(); 
	//Dibujo la tetera lado derecho
	glPushAttrib(GL_ALL_ATTRIB_BITS);
		glStencilFunc(GL_EQUAL,1,~0); 
		glStencilOp(GL_KEEP,GL_KEEP,GL_KEEP);
		toon=true;
		drawTeapot();
	glPopAttrib();
	
	//Dibujo la tetera lado izquierdo
	glPushAttrib(GL_ALL_ATTRIB_BITS);
		glStencilFunc(GL_EQUAL,2,~0); 
		glStencilOp(GL_KEEP,GL_KEEP,GL_KEEP);
		toon=false;
		drawTeapot();
	glPopAttrib();

	//Dibujo el plano
	
	glPushAttrib(GL_ALL_ATTRIB_BITS);
		glDisable(GL_LIGHTING);
		glDisable(GL_LIGHT0);
		glUseProgram(0);
		glColor3f(0,0,0);
		glPushMatrix();	//matrix del separador
			if(boton==GLUT_LEFT_BUTTON && mov_plano) aux=movimientoX;
			glRotatef(-amy,0,1,0);
			glScalef(1,1.5,1.5);
			glTranslated(aux/200,0,0);
			separador();
		glPopMatrix();
	glPopAttrib();
	
	glutSwapBuffers();
}

void regen() {
	if (!dibuja) return;
	
	// matriz de proyeccion
	glMatrixMode(GL_PROJECTION);  glLoadIdentity();
	
	double w0=(double)w/2/escala,h0=(double)h/2/escala; // semiancho y semialto en el target
	
	// frustum
	double
		delta[3]={(target[0]-eye[0]), // vector ojo-target
		(target[1]-eye[1]),
		(target[2]-eye[2])},
			dist=sqrt(delta[0]*delta[0]+
					  delta[1]*delta[1]+
					  delta[2]*delta[2]);
	w0*=znear/dist,h0*=znear/dist;  // w0 y h0 en el near
	glFrustum(-w0,w0,-h0,h0,znear,zfar);
	
	glMatrixMode(GL_MODELVIEW); glLoadIdentity(); // matriz del modelo->view
	
	if (luz_fija) glLightfv(GL_LIGHT0,GL_POSITION,lpos);  // ubica la luz
	gluLookAt(   eye[0],   eye[1],   eye[2],
			  target[0],target[1],target[2],
			  up[0],    up[1],    up[2]);// ubica la camara
	
	glRotatef(amy,0,1,0); // rota los objetos alrededor de y
	
	if (!luz_fija) glLightfv(GL_LIGHT0,GL_POSITION,lpos);  // ubica la luz
	
	glutPostRedisplay(); // avisa que hay que redibujar
}

void Idle_cb() {
	// Cuenta el lapso de tiempo
	static int anterior=glutGet(GLUT_ELAPSED_TIME); // milisegundos desde que arranco
	int tiempo=glutGet(GLUT_ELAPSED_TIME), lapso=tiempo-anterior;
	if (lapso<msecs) Sleep(msecs-lapso); // ademas de controlar la velocidad, el sleep evita la busy-wait (freir el micro esperando)
	anterior=tiempo;
	if (rota) { amy+=2; if (amy>360) amy-=360; regen(); }
	glutPostRedisplay(); // redibuja
}

void reshape_cb (int width, int height) {
	h=height; w=width;
	if (w==0||h==0) {// minimiza
		dibuja=false; // no dibuja mas
		glutIdleFunc(0); // no llama a cada rato a esa funcion
		return;
	}
	else if (!dibuja&&w&&h){// des-minimiza
		dibuja=true; // ahora si dibuja
		glutIdleFunc(Idle_cb); // registra de nuevo el callback
	}
	glViewport(0,0,w,h); // region donde se dibuja
	regen(); //regenera la matriz de proyeccion
	glutPostRedisplay();
}

void Keyboard_cb(unsigned char key,int x=0,int y=0) {
	switch (key){
	case 'f': case 'F': // relleno
		relleno=!relleno;
		break;
	case 'w': case 'W': // wire
		wire=!wire;
		break;
	case 'l': case 'L': // posicion de la luz
		luz_fija = !luz_fija;
		regen();
		break;
	case 'd': case 'D': // posicion de la luz
		if (lpos[3]==0) lpos[3]=1;
		else lpos[3]=0;
		break;
	case 'a': case 'A': // transparencia
		blend=!blend;
		if (blend){
			glEnable(GL_BLEND);
			relleno=false;
		}
		else {
			glDisable(GL_BLEND);
			relleno=true;
		}
		break;
	case 'p': case 'P': 
		mov_plano=!mov_plano;
		break;
	case 'r': case 'R': // rotacion
		rota=!rota;
		break;
	case 't': case 'T': // rotacion
		toon=!toon;
		break;
	case 'g': case 'G': // smooth
		smooth=!smooth;
		glShadeModel((smooth) ? GL_SMOOTH : GL_FLAT);
		break;
	case '+': case '-': // lod
		if (key=='+') lod++; else {lod--; if (lod==0) lod=1;}
		break;
	}
}

void Menu_cb(int value){
	Keyboard_cb(value);
}

// Movimientos del mouse
void Motion_cb(int xm, int ym){ // drag
	movimientoX=xm-w/2; //SACO EL VALOR PARA MOVER EL SEPARADOR
	
	if (boton==GLUT_LEFT_BUTTON && !mov_plano){
		double yc=eye[1]-target[1],zc=eye[2]-target[2];
		double ac=ac0+(ym-yclick)*180.0/h/R2G;
		yc=rc0*sin(ac); zc=rc0*cos(ac);
		
		up[1]=zc; //up[2]=-yc;  // perpendicular
		eye[1]=target[1]+yc; eye[2]=target[2]+zc;
		amy=amy0+(xm-xclick)*180.0/w;
		regen();
	}
}

void Mouse_cb(int button, int state, int x, int y){
	static bool old_rota=false;
	if (button==GLUT_LEFT_BUTTON){
	if (state==GLUT_DOWN ) {
			xclick=x; yclick=y;
			
			boton=button;
			old_rota=rota; rota=false;
			glutMotionFunc(Motion_cb); // callback para los drags
			
			double yc=eye[1]-target[1],zc=eye[2]-target[2];
			rc0=sqrt(yc*yc+zc*zc); ac0=atan2(yc,zc);
			amy0=amy;
		}
		else if (state==GLUT_UP){
			rota=old_rota;
			boton=-1;
			glutMotionFunc(0); // anula el callback para los drags
		}
	}
}

void initialize() {
	glutInitDisplayMode(GLUT_DEPTH|GLUT_RGBA|GLUT_DOUBLE);
	glutInitWindowSize(w,h);
	glutInitWindowPosition(50,50);
	glutCreateWindow ("Toon View");
	
	//Declaro los shaders
	glewInit();
	extern GLuint setShaders();
	ProgID = setShaders();
	
	//Declaro las funciones que se usan
	glutDisplayFunc (display_cb);
	glutReshapeFunc (reshape_cb);
	glutKeyboardFunc(Keyboard_cb);
	glutIdleFunc(Idle_cb);
	glutMouseFunc(Mouse_cb);
	
	//Declaro el estado de inicializacion
	glEnable(GL_STENCIL_TEST);													//meto stencil test washin
	glEnable(GL_DEPTH_TEST); glDepthFunc(GL_LEQUAL); // habilita el z-buffer
	glEnable(GL_NORMALIZE); // para que el scaling no moleste
	glEnable(GL_POLYGON_OFFSET_FILL); glPolygonOffset (1,1); // coplanaridad
	glShadeModel((smooth) ? GL_SMOOTH : GL_FLAT); // interpola normales por nodos o una normal por plano
	
	// ancho de lineas y puntos
	glLineWidth(linewidth);
	glPointSize(pointsize);
	
	// antialiasing
	glHint(GL_POINT_SMOOTH_HINT,GL_NICEST);
	glHint(GL_LINE_SMOOTH_HINT,GL_NICEST);
	glHint(GL_POLYGON_SMOOTH_HINT,GL_NICEST);
	
	// transparencias
	if (blend) glEnable(GL_BLEND);
	else glDisable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	
	// color de fondo
	glClearColor(1.f,1.f,1.f,0);
	
	// direccion de los poligonos
	glFrontFace(GL_CCW);
	glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
	glCullFace(GL_BACK);
	
	//Defino las luces
	
	glUseProgram(ProgID);
	glColor3f(0,1.f,0);
	
	//Luces OpenGL
	static const float
	lambient[]={.1f,.1f,.1f,1}, // luz ambiente
	ldiffuse[]={.65f,.65f,.65f,1}, // luz difusa
	lspecular[]={1,1,1,1};      // luz especular
	glLightfv(GL_LIGHT0,GL_AMBIENT,lambient);
	glLightfv(GL_LIGHT0,GL_DIFFUSE,ldiffuse);
	glLightfv(GL_LIGHT0,GL_SPECULAR,lspecular);
	// caras de atras y adelante distintos (1) o iguales (0)
	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE,1);
	glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER,0);
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHTING);
	
	
	// material estandar
	static const float
		fambient[]={.4f,.2f,.1f,1},
		fdiffuse[]={.5f,.2f,.1f,1},
		fspecular[]={1,1,1,1},
		bambient[]={.2f,.4f,.4f,1},
		bdiffuse[]={.2f,.5f,.4f,1},
		bspecular[]={1,1,1,1};
	static const int
		fshininess=75,
		bshininess=100;
	glMaterialfv(GL_FRONT,GL_AMBIENT,fambient);
	glMaterialfv(GL_FRONT,GL_DIFFUSE,fdiffuse);
	glMaterialfv(GL_FRONT,GL_SPECULAR,fspecular);
	glMateriali(GL_FRONT,GL_SHININESS,fshininess);
	glMaterialfv(GL_BACK,GL_AMBIENT,bambient);
	glMaterialfv(GL_BACK,GL_DIFFUSE,bdiffuse);
	glMaterialfv(GL_BACK,GL_SPECULAR,bspecular);
	glMateriali(GL_BACK,GL_SHININESS,bshininess);
	glEnable(GL_COLOR_MATERIAL);
	glColorMaterial(GL_FRONT,GL_AMBIENT_AND_DIFFUSE);
	
	// crea el menu
	glutCreateMenu(Menu_cb);
	glutAddMenuEntry("[f]_Caras Rellenas              ", 'f');
	glutAddMenuEntry("[l]_Luz fija/movil              ", 'l');
	glutAddMenuEntry("[d]_Luz direccional/Posicional  ", 'l');
	glutAddMenuEntry("[w]_Wireframe                   ", 'w');
	glutAddMenuEntry("[r]_Rota                        ", 'r');
	glutAddMenuEntry("[a]_Transparencia               ", 'a');
	glutAddMenuEntry("[t]_Activar/Desactivar Toon     ", 't');
	glutAddMenuEntry("[g]_Flat/Gouraud                ", 's');
	glutAddMenuEntry("[+]_+Nivel de Detalle           ", '+');
	glutAddMenuEntry("[-]_-Nivel de Detalle           ", '-');
	glutAttachMenu(GLUT_RIGHT_BUTTON);
	
	regen(); // para que setee las matrices antes del 1er draw
}

int main (int argc, char **argv) {
	glutInit (&argc, argv);
	initialize();
	glutMainLoop();
	return 0;
}
