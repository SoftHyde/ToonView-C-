//Fragment Shader
//Realizo la deteccion de contorno
#version 110

varying float color_pred;
varying vec3 transformedNormal, posicion_ojo, ecPosition3, direccion_luz, posicion_luz, direccion_obs;
varying vec4 AmbientFront, AmbientBack;
varying vec4 DiffuseFront, DiffuseBack;
varying vec4 SpecularFront, SpecularBack;

vec3 normal;
vec4 color=vec4 (0.0);

float intensity;
float niveles_color = 5.0; //Establezco la cantidad de niveles de color
float scaleFactor = 1.0 / niveles_color;

// Luz direccional
void directionalLight(in bool front){
	color=vec4 (0.0);
	//Luz ambiental
	vec4 Ambient=((front)? AmbientFront : AmbientBack);
	//Luz difusa
	vec4 Diffuse=((front)? DiffuseFront : DiffuseBack);
	float angulo_luz_normal=max(0.0, dot(normal, direccion_luz)); // si el coseno es menor que 0 no ilumina
	//Luz especular
	vec4 Specular=((front)? SpecularFront : SpecularBack);
	float shininess=((front)? gl_FrontMaterial.shininess : gl_BackMaterial.shininess);
	vec3 VecH=normalize(direccion_luz + posicion_ojo);
	float angulo_normal_h=max(0.0, dot(VecH, normal)); // normal productopunto light half vector (n.h)
	float pf;     // shininess (reflexión no ideal de Phong)
	if(angulo_luz_normal > 0.0){
		pf=pow(angulo_normal_h, shininess);
	}else{
		pf=0.0;
	}
	
	//Defino el color
	float specMask = (angulo_normal_h > 0.3) ? 1.0 : 0.0;
	float edge_detection = (dot(direccion_obs, normal)>0.25) ? 1.0 : 0.0; //si el angulo entre la direccion del observador y la normal (funcion dot) es menor a 0,3 asigna 0 (negro)
	color=  Ambient * edge_detection;
	color+= Diffuse * floor(angulo_luz_normal*niveles_color)*scaleFactor * edge_detection;  //el floor hace que nos quede un numero entero entre 0 y 5 y el scale factor convierte ese numero en una proporcion entre 0 y 1
	color+= Specular * specMask *floor(pf*niveles_color) * edge_detection;
}

// Luz puntual
void pointLight(in bool front){
	color=vec4 (0.0);
	vec3 lightPn=normalize(posicion_luz);
	float attenuation; // factor de atenuacion
	float d=length(posicion_luz); // distancia entre la superficie y la posicion de la luz (modulo del vector)
	// atenuacion
	attenuation=1.0 / (gl_LightSource[0].constantAttenuation + 
					   gl_LightSource[0].linearAttenuation * d + 
					   gl_LightSource[0].quadraticAttenuation * d * d);
	
	//Luz ambiental
	vec4 Ambient=((front)? AmbientFront : AmbientBack);
	//Luz difusa
	vec4 Diffuse=((front)? DiffuseFront : DiffuseBack);
	float angulo_luz_normal=max(0.0, dot(normal, lightPn)); // si el coseno es menor que 0 no ilumina
	//Luz especular
	vec4 Specular=((front)? SpecularFront : SpecularBack);
	float shininess=((front)? gl_FrontMaterial.shininess : gl_BackMaterial.shininess);
	vec3 VecH=normalize(lightPn + posicion_ojo);
	float angulo_normal_h=max(0.0, dot(VecH, normal)); // normal productopunto light half vector (n.h)
	float pf;     // shininess (reflexión no ideal de Phong)
	if(angulo_luz_normal > 0.0){
		pf=pow(angulo_normal_h, shininess);
	}else{
		pf=0.0;
	}
	
	//Defino el color
	float specMask = (angulo_normal_h > 0.3) ? 1.0 : 0.0;
	float edge_detection = (dot(direccion_obs, normal)>0.25) ? 1.0 : 0.0; //si el angulo entre la direccion del observador y la normal (funcion dot) es menor a 0,3 asigna 0 (negro)
	color=  Ambient * attenuation * edge_detection;
	color+= Diffuse * attenuation * edge_detection * floor(angulo_luz_normal*niveles_color) * scaleFactor; //el floor hace que nos quede un numero entero entre 0 y 5 y el scale factor convierte ese numero en una proporcion entre 0 y 1
	color+= Specular * floor(pf*niveles_color) *attenuation * edge_detection * specMask;
}

/*void directionalLight(in bool front){
	//vec4 color;
	intensity = dot(direccion_luz,normal); //El producto me permite obtener la normal transformada y asi el efecto de la luz se mueva con respecto al movimiento de la camara
	if (color_pred==0.0){
		if (intensity > 0.95)
			color = vec4(1.0,0.5,0.5,1.0);
		else if (intensity > 0.75)
			color = vec4(0.75,0.4,0.4,1.0);
		else if (intensity > 0.5)
			color = vec4(0.5,0.3,0.3,1.0);
		else if (intensity > 0.25)
			color = vec4(0.3,0.2,0.2,1.0);
		else
			color = vec4(0.2,0.1,0.1,1.0);
	}
	else if (color_pred==1.0){
		if (intensity > 0.95)
			color = vec4(0.5,1.0,0.5,1.0);
		else if (intensity > 0.75)
			color = vec4(0.4,0.75,0.4,1.0);
		else if (intensity > 0.5)
			color = vec4(0.3,0.5,0.3,1.0);
		else if (intensity > 0.25)
			color = vec4(0.2,0.3,0.2,1.0);
		else
			color = vec4(0.1,0.2,0.1,1.0);
	}
	else{
		if (intensity > 0.95)
			color = vec4(0.5,0.5,1.0,1.0);
		else if (intensity > 0.75)
			color = vec4(0.4,0.4,0.75,1.0);
		else if (intensity > 0.5)
			color = vec4(0.3,0.3,0.5,1.0);
		else if (intensity > 0.25)
			color = vec4(0.2,0.2,0.3,1.0);
		else
			color = vec4(0.1,0.1,0.2,1.0);
	}
	float edge_detection = (dot(direccion_obs, normal)>0.3) ? 1.0 : 0.0; //si el angulo entre la direccion del observador y la normal (funcion dot) es menor a 0,3 asigna 0 (negro)
	color=edge_detection*color;
}
	
void pointLight(in bool front){
	vec3 luz_pos_norm=normalize(posicion_luz);
	intensity = dot(luz_pos_norm,normal); //El producto me permite obtener la normal transformada y asi el efecto de la luz se mueva con respecto al movimiento de la camara
	
	if (color_pred==0.0){
		if (intensity > 0.95)
			color = vec4(1.0,0.5,0.5,1.0);
		else if (intensity > 0.75)
			color = vec4(0.75,0.4,0.4,1.0);
		else if (intensity > 0.5)
			color = vec4(0.5,0.3,0.3,1.0);
		else if (intensity > 0.25)
			color = vec4(0.3,0.2,0.2,1.0);
		else
			color = vec4(0.2,0.1,0.1,1.0);
	}
	else if (color_pred==1.0){
		if (intensity > 0.95)
			color = vec4(0.5,1.0,0.5,1.0);
		else if (intensity > 0.75)
			color = vec4(0.4,0.75,0.4,1.0);
		else if (intensity > 0.5)
			color = vec4(0.3,0.5,0.3,1.0);
		else if (intensity > 0.25)
			color = vec4(0.2,0.3,0.2,1.0);
		else
			color = vec4(0.1,0.2,0.1,1.0);
	}
	else{
		if (intensity > 0.95)
			color = vec4(0.5,0.5,1.0,1.0);
		else if (intensity > 0.75)
			color = vec4(0.4,0.4,0.75,1.0);
		else if (intensity > 0.5)
			color = vec4(0.3,0.3,0.5,1.0);
		else if (intensity > 0.25)
			color = vec4(0.2,0.2,0.3,1.0);
		else
			color = vec4(0.1,0.1,0.2,1.0);
	}
	float edge_detection = (dot(direccion_obs, normal)>0.3) ? 1.0 : 0.0; //si el angulo entre la direccion del observador y la normal (funcion dot) es menor a 0,3 asigna 0 (negro)
	color=edge_detection*color;
}*/

// iluminacion de caras frontales y traseras
void fblightF(){
	normal=normalize(transformedNormal);
	// el fragmento pertenece a la cara frontal
	if(gl_FrontFacing){
		if (gl_LightSource[0].position.w == 0.0){
			directionalLight(true);  
		}else{
			pointLight(true);   
		}
	}
	else{
		// el fragmento pertenece a la cara trasera
		//normal=-normal;    
		if (gl_LightSource[0].position.w == 0.0){
			directionalLight(false);  
		}else{
			pointLight(false);
		}
	}
	gl_FragColor=color;
}
	
void main (void){
	fblightF();
}
