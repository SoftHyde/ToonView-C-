// Sombreado Phong Shading: Normales interpoladas y cálculo de iluminación en cada fragmento - Vertex shader 
#version 110

uniform bool LocalViewer;
varying float color_pred;
varying vec3 transformedNormal, posicion_ojo, ecPosition3, direccion_luz, posicion_luz, direccion_obs;


float luz_predominante(){
	if((gl_FrontMaterial.ambient[0]*gl_FrontMaterial.diffuse[0])>(gl_FrontMaterial.ambient[1]*gl_FrontMaterial.diffuse[1])){
		if((gl_FrontMaterial.ambient[0]*gl_FrontMaterial.diffuse[0])>(gl_FrontMaterial.ambient[2]*gl_FrontMaterial.diffuse[2])){
			return 0.0;
		}
	}
	else if((gl_FrontMaterial.ambient[1]*gl_FrontMaterial.diffuse[1])>(gl_FrontMaterial.ambient[2]*gl_FrontMaterial.diffuse[2])){
		return 1.0;
	}
	else{
		return 2.0;
	}
}


varying vec4 AmbientFront, AmbientBack;
varying vec4 DiffuseFront, DiffuseBack;
varying vec4 SpecularFront, SpecularBack;

// iluminacion de caras frontales y traseras
void fblightV(){
  // estos calculos pueden ser realizados en el vertex shader
  // caras frontales
  // inicializar componentes
  AmbientFront =vec4 (0.0);
  DiffuseFront =vec4 (0.0);
  SpecularFront=vec4 (0.0);
  AmbientFront= gl_FrontMaterial.ambient * gl_LightModel.ambient;// gl_FrontLightModelProduct.sceneColor
  AmbientFront+=gl_FrontMaterial.ambient * gl_LightSource[0].ambient;
  DiffuseFront=gl_FrontMaterial.diffuse * gl_LightSource[0].diffuse;
  SpecularFront=gl_FrontMaterial.specular * gl_LightSource[0].specular;
  
  // caras traseras
  AmbientBack =vec4 (0.0);
  DiffuseBack =vec4 (0.0);
  SpecularBack=vec4 (0.0);
  AmbientBack= gl_BackMaterial.ambient * gl_LightModel.ambient;// gl_BackLightModelProduct.sceneColor
  AmbientBack+=gl_BackMaterial.ambient * gl_LightSource[0].ambient;
  DiffuseBack=gl_BackMaterial.diffuse * gl_LightSource[0].diffuse;
  SpecularBack=gl_BackMaterial.specular * gl_LightSource[0].specular;
}


void main (void){
  // Computa la normal
  transformedNormal=normalize(gl_NormalMatrix * gl_Normal);
  // Luz direccional
  direccion_luz=normalize(vec3(gl_LightSource[0].position));
  // posicion de los vertices en coordenadas del ojo
  vec4 ecPosition=gl_ModelViewMatrix * gl_Vertex;
  // posicion 3D
  ecPosition3=(vec3(ecPosition))/ecPosition.w;
  // Luz puntual: vector desde la posicion de la luz hasta la superficie
  posicion_luz=vec3(gl_LightSource[0].position) - ecPosition3;
  
  if(LocalViewer){
	posicion_ojo=normalize(-ecPosition3); // {0,0,0}-{x,y,z} // invertimos el vector ojo
  }
  else{
	posicion_ojo=vec3(0.0,0.0,1.0); // localViewer false ==> unitario en z
  }
  
  // Direccion observador
  direccion_obs=normalize(posicion_ojo - ecPosition3);
  color_pred=luz_predominante();
  
  // iluminacion de caras frontales y traseras
  fblightV();
  
  // Vertices transformados a la functionalidad fija
  gl_Position=gl_ProjectionMatrix * gl_ModelViewMatrix * gl_Vertex; // gl_Position=ftransform();
}
