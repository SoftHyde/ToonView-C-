# include <GL/glew.h>
# include <GL/gl.h>
#include <cstdlib>
#include <iostream>
#include <fstream>
using namespace std;

//Funcion para leer el contenido de los shaders
char *textFileRead(char *fn) {
	FILE *fp;
	char *content = NULL;
	
	int count=0;
	
	if (fn != NULL) {
		fp = fopen(fn,"rt");
		
		if (fp != NULL) {
			
			fseek(fp, 0, SEEK_END);
			count = ftell(fp);
			rewind(fp);
			
			if (count > 0) {
				content = (char *)malloc(sizeof(char) * (count+1));
				count = fread(content,sizeof(char),count,fp);
				content[count] = '\0';
			}
			fclose(fp);
		}
	}
	
	if (content == NULL) {
		fprintf(stderr, "ERROR: No se pudo abrir el archivo %s\n", fn);
		exit(1);
	}
	return content;
} 

// imprime la info del compilador de la GPU
void CompilerInfo(GLuint id){
  int len; glGetShaderiv(id,GL_INFO_LOG_LENGTH,&len); // cant de caracteres
  if (len){
    char * infoLog=(char *)malloc(sizeof(char)*(len+1));
    glGetShaderInfoLog(id,len+1,NULL,infoLog);
    cout << infoLog << endl;
    free(infoLog);
  }
  int tipo; glGetShaderiv(id,GL_SHADER_TYPE,&tipo);
  int status; glGetShaderiv(id,GL_COMPILE_STATUS,&status);
  cout << ((tipo==GL_VERTEX_SHADER)? "Vertex" : "Fragment") 
    << " Shader Compilado " << ((status)? "C" : "Inc") << "orrectamente\n";
}

// imprime la info del linker de la GPU
void LinkerInfo(GLuint id){
  int len; glGetProgramiv(id,GL_INFO_LOG_LENGTH,&len); // cant de caracteres
  if (len){
    char * infoLog=(char *)malloc(sizeof(char)*(len+1));
    glGetProgramInfoLog(id,len+1,NULL,infoLog);
    cout << infoLog << endl;
    free(infoLog);
  }
  int status; glGetProgramiv(id,GL_LINK_STATUS,&status);
  cout << "Programa Linkeado " << ((status)? "C" : "Inc") << "orrectamente\n";
}

GLuint setShaders() {
  
  // Crea los shaders
  GLuint v = glCreateShader(GL_VERTEX_SHADER);
  GLuint f = glCreateShader(GL_FRAGMENT_SHADER);
  
  char *vs = textFileRead("lighting.vert"); const char *vsptr=vs;
  char *fs = textFileRead("lighting.frag"); const char *fsptr=fs;
  
  // asigna una fuente a un shader, la fuente puede ser un array de strings
  // para saber el tamaño de c/u se necesita tambien un array de longitudes
  //glShaderSource(shader,cant de strings,puntero,puntero al array de long)
  glShaderSource(v,1,&vsptr,NULL);
  glShaderSource(f,1,&fsptr,NULL);
  // los strings ya pasaron a la placa gráfica
  free(vs); free(fs);
  
  // pide a la GPU que compile 
  glCompileShader(v); CompilerInfo(v);
  glCompileShader(f); CompilerInfo(f);
  
  // crea el programa
  GLuint progID = glCreateProgram();
  glAttachShader(progID,v);
  glAttachShader(progID,f);
  
  // Linkea todo en el programa
  glLinkProgram(progID); LinkerInfo(progID);
  
  glUniform1i(glGetUniformLocation(progID, "LocalViewer"), true);
  
  return progID;
}
