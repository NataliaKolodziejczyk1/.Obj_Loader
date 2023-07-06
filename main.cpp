#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "GL\glew.h"
#include "GL\freeglut.h"

#include "shaderLoader.h" //ładowanie i kompilacja shaderów
#include "Objloader.h"
#include "tekstura.h"

//funkcje algebry liniowej
#include "glm/vec3.hpp" // glm::vec3
#include "glm/vec4.hpp" // glm::vec4
#include "glm/mat4x4.hpp" // glm::mat4
#include "glm/gtc/matrix_transform.hpp" // glm::translate, glm::rotate, glm::scale, glm::perspective

#include <vector>

//Wymiary okna
int screen_width = 640;
int screen_height = 480;


int pozycjaMyszyX; // na ekranie
int pozycjaMyszyY;
int mbutton; // wcisiety klawisz myszy

//pozycja kamery
double kameraX= -30.0;
double kameraZ = 10.0;
double kameraD = -250.0;
double kameraPredkosc;
double kameraKat = 20;
double kameraPredkoscObrotu;
double poprzednie_kameraX;
double poprzednie_kameraZ;
double poprzednie_kameraD;
double translation_vertical;
double movement_x=0;
double movement_y=0;
double movement_z=0;

double rotation = 0;

//zmienne dla tekstur
GLint uniformTex0;
GLuint  tex_id0;

//macierze
glm::mat4 MV; //modelview - macierz modelu i świata
glm::mat4 P;  //projection - macierz projekcji, czyli naszej perspektywy

GLuint size; // ilość wierzchołków wczytanych z pliku

float translation_x,translation_y,translation_z; // przesunięcie obiektu
float x_axis,y_axis,z_axis; // długość osi

// krok siatki
float x_step,y_step,z_step;


//tryby działania programu
int col=0;
int tex=0;
int net=1;


//shaders
GLuint programID = 0;
GLuint programID2 = 0;
GLuint programID3 = 0;

unsigned int VBO;
unsigned int VAO[1];

/*###############################################################*/

//funckja licząca, co ile ma pojawić się siatka na danej osi
float CountStep(float axis_size)
{
	if(axis_size <= 10) return 2.5;
	else if(axis_size <= 30) return 5;
	else if(axis_size <= 60) return 10;
	else if(axis_size <= 90) return 15;
	else if(axis_size <= 140) return 20;
	else if(axis_size <= 200) return 25;
	return 50;
}

void DrawTekst(const char* tekst,float x,float y,float z) //funkcja do wypisywania tekstu (Która z jakiegoś powodu nie działa)
{
	glRasterPos3f(x,y,z); // Ustaw pozycję rasteryzacji tekstu
	glColor3f(0.0f,0.0f,0.0f);
	for(const char* znak = tekst; *znak != '\0'; znak++)
	{
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_10,*znak); // Wyświetl pojedynczy znak tekstu
	}
}

void mysz(int button, int state, int x, int y) // obsługa myszki
{
	mbutton = button;
	switch (state)
	{
	case GLUT_UP:
		break;
	case GLUT_DOWN:
		pozycjaMyszyX = x;
		pozycjaMyszyY = y;
		poprzednie_kameraX = kameraX;
		poprzednie_kameraZ = kameraZ;
		poprzednie_kameraD = kameraD;
		break;

	}
}
/*******************************************/
void mysz_ruch(int x, int y) // obsługa myszki
{
	if (mbutton == GLUT_LEFT_BUTTON)
	{
		kameraX = poprzednie_kameraX - (pozycjaMyszyX - x) * 0.1;
		kameraZ = poprzednie_kameraZ - (pozycjaMyszyY - y) * 0.1;
	}
	if (mbutton == GLUT_RIGHT_BUTTON)
	{
		kameraD = poprzednie_kameraD + (pozycjaMyszyY - y) * 0.1;
	}

}
/******************************************/

void specialKeys(int key,int x,int y)
{
	switch(key) {
	case GLUT_KEY_UP:
		movement_y+=1;
		break;
	case GLUT_KEY_DOWN:
		movement_y-=1;
		break;
	case GLUT_KEY_LEFT:
		movement_x-=1;
		break;
	case GLUT_KEY_RIGHT:
		movement_x+=1;
		break;
	}
}

/******************************************/
void klawisz(GLubyte key, int x, int y) // obsługa klawiatury
{
	switch(key) {

	case 27:    /* Esc - koniec */
		exit(1);
		break;
	case 'n': //włączenie/wyłączenie siatki
		if(net == 1) net = 0;
		else net = 1;
		break;
	case 't': //włączenie/wyłączenie tekstury
		if(tex == 1) tex = 0;
		else {
			if(col == 1) col = 0;
			tex =1;
		}
		break;
	case 'c': //włączenie/wyłączenie wypełnienia kolorem
		if(col == 1) col = 0;
		else {
			if(tex == 1) tex = 0;
			col =1;
		}
		break;
	case 'a':
		movement_z-=1;
		break;
	case 'd':
		movement_z+=1;
		break;
	case 'r':
		movement_z=0;
		movement_x=0;
		movement_y=0;
		if(z_axis > x_axis)
		{
			kameraD = -z_axis*1.7;
		}
		else
		{
			kameraD = -x_axis*1.7;
		}
		kameraX= -30.0;
		kameraZ = 10.0;
		break;
	}
	
	
}
/*###############################################################*/
void rysuj(void)
{

	glClearColor(1.0f,1.0f,1.0f,1.0f); //kolor tła 
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Kasowanie ekranu

	glUseProgram(programID);


	MV = glm::mat4(1.0f);  //macierz jednostkowa
	MV = glm::translate(MV,glm::vec3(0.0f,translation_vertical,kameraD));
	MV = glm::rotate(MV,(float)glm::radians(kameraZ),glm::vec3(1,0,0));
	MV = glm::rotate(MV,(float)glm::radians(kameraX),glm::vec3(0,1,0));

	glm::mat4 MVP = P * MV;
	MVP = P * MV; //początkowe ustawienie

	GLuint MVP_id = glGetUniformLocation(programID, "MVP"); // pobierz lokalizację zmiennej 'uniform' "MV" w programie
	glUniformMatrix4fv(MVP_id, 1, GL_FALSE, &(MVP[0][0]));	   // wyślij tablicę mv do lokalizacji "MV", która jest typu mat4	

	uniformTex0 = glGetUniformLocation(programID,"tex0"); 
	glUniform1i(uniformTex0,0);
	
	//DrawTekst("X",10.0f,0.0f,0.0f); // Etykieta osi X 

	MV = glm::translate(MV,glm::vec3(-translation_x+movement_x,-translation_y+movement_y,-translation_z+movement_z)); // przesunięcie obiektu do dodatnich ćwiartek układu współrzędnych
	MVP = P * MV;
	MVP_id = glGetUniformLocation(programID,"MVP");
	glUniformMatrix4fv(MVP_id,1,GL_FALSE,&(MVP[0][0]));

	// Rysowanie siatki
	if(net == 1)
	{
		glBindVertexArray(VAO[0]);
		glLineWidth(1.0f);
		GLfloat attrib[] ={0.0f,0.0f,0.0f}; // zmiana koloru na czarny
		glVertexAttrib3fv(3,attrib);
		glPolygonMode(GL_FRONT_AND_BACK,GL_LINE); // tryb linii
		glDrawArrays(GL_TRIANGLES,0,size);
	}

	MV = glm::translate(MV,glm::vec3(translation_x,translation_y,translation_z)); // wracamy na środek układu współrzędnych
	MVP = P * MV;
	MVP_id = glGetUniformLocation(programID,"MVP"); // pobierz lokalizację zmiennej 'uniform' "MV" w programie
	glUniformMatrix4fv(MVP_id,1,GL_FALSE,&(MVP[0][0]));
	glLineWidth(1.5f); // Ustaw grubość linii

	 // Oś X 
	
	glBegin(GL_LINES);
	glVertex3f(0.0f,0.0f,z_axis+5.0f); // Początek linii
	glVertex3f(x_axis+5.0f,0.0f,z_axis+5.0f); // Koniec linii
	glEnd();

	// Oś Y

	glBegin(GL_LINES);
	glVertex3f(0.0f,0.0f,z_axis+5.0f); // Początek linii
	glVertex3f(0.0f,y_axis+5.0f,z_axis+5.0f); // Koniec linii
	glEnd();
	
	// Oś Z

	glBegin(GL_LINES);
	glVertex3f(5.0f+x_axis,0.0f,0.0f); // Początek linii
	glVertex3f(5.0f+x_axis,0.0f,z_axis+5.0f); // Koniec linii
	glEnd();

	
	//Siatka X
	float dist = x_axis+5.0f-x_step;
	while(dist >= 0)
	{
		//siatka
		glLineWidth(1.0f); // Ustaw grubość linii
		glPushAttrib(GL_ENABLE_BIT);
		glLineStipple(1,0xAAAA);
		glEnable(GL_LINE_STIPPLE);

		glBegin(GL_LINES);
		glVertex3f(dist,0.0f,0.0f); // Początek linii
		glVertex3f(dist,y_axis+5.0f,0.0f); // Koniec linii
		glEnd();

		glBegin(GL_LINES);
		glVertex3f(dist,0.0f,0.0f); // Początek linii
		glVertex3f(dist,0.0f,z_axis+5.0f); // Koniec linii
		glEnd();

		glPopAttrib();

		//znacznik na osi
		glLineWidth(1.5f);

		glBegin(GL_LINES);
		glVertex3f(dist,0.0f,z_axis+5.0f); // Początek linii
		glVertex3f(dist,0.0f,z_axis+7.0f); // Koniec linii
		glEnd();

		
		dist -= x_step;
	}

	//Siatka Y
	dist = 0;
	while(dist <= y_axis+5.0f)
	{
		//siatka
		glLineWidth(1.0f); // Ustaw grubość linii
		glPushAttrib(GL_ENABLE_BIT);
		glLineStipple(1,0xAAAA);
		glEnable(GL_LINE_STIPPLE);

		glBegin(GL_LINES);
		glVertex3f(0.0f,dist,0.0f); // Początek linii
		glVertex3f(x_axis+5.0f,dist,0.0f); // Koniec linii
		glEnd();

		glBegin(GL_LINES);
		glVertex3f(0.0f,dist,0.0f); // Początek linii
		glVertex3f(0.0f,dist,z_axis+5.0f); // Koniec linii
		glEnd();

		glPopAttrib();

		//znacznik na osi
		glLineWidth(1.5f);

		glBegin(GL_LINES);
		glVertex3f(0.0f,dist,z_axis+5.0f); // Początek linii
		glVertex3f(0.0f,dist,z_axis+7.0f); // Koniec linii
		glEnd();


		dist += y_step;
	}

	//Siatka Z
	dist = z_axis+5.0f-z_step;
	while(dist >= 0)
	{
		//siatka
		glLineWidth(1.0f); // Ustaw grubość linii
		glPushAttrib(GL_ENABLE_BIT);
		glLineStipple(1,0xAAAA);
		glEnable(GL_LINE_STIPPLE);

		glBegin(GL_LINES);
		glVertex3f(0.0f,0.0f,dist); // Początek linii
		glVertex3f(x_axis+5.0f,0.0f,dist); // Koniec linii
		glEnd();

		glBegin(GL_LINES);
		glVertex3f(0.0f,0.0f,dist); // Początek linii
		glVertex3f(0.0f,y_axis+5.0f,dist); // Koniec linii
		glEnd();

		glPopAttrib();

		//znacznik na osi
		glLineWidth(1.5f);

		glBegin(GL_LINES);
		glVertex3f(x_axis+5.0f,0.0f,dist); // Początek linii
		glVertex3f(x_axis+7.0f,0.0f,dist); // Koniec linii
		glEnd();


		dist -= z_step;
	}
	// Wypełnienie teksturą
	if(tex == 1)
	{
		glUseProgram(programID2);
		MV = glm::translate(MV,glm::vec3(-translation_x,-translation_y,-translation_z));
		MVP = P * MV;
		GLuint MVP_id2 = glGetUniformLocation(programID2,"MVP"); // pobierz lokalizację zmiennej 'uniform' "MV" w programie
		glUniformMatrix4fv(MVP_id2,1,GL_FALSE,&(MVP[0][0]));
		glBindVertexArray(VAO[0]);
		GLfloat attrib[] ={0.0f,0.0f,0.0f};
		glVertexAttrib3fv(3,attrib);
		glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
		glDrawArrays(GL_TRIANGLES,0,size);
	}

	//Wypełnianie kolorem
	if(col == 1)
	{
		glUseProgram(programID3); //u┐yj programu, czyli naszego shadera	
		MV = glm::translate(MV,glm::vec3(-translation_x,-translation_y,-translation_z));
		MVP = P * MV;
		GLuint MVP_id2 = glGetUniformLocation(programID3,"MVP"); // pobierz lokalizację zmiennej 'uniform' "MV" w programie
		glUniformMatrix4fv(MVP_id2,1,GL_FALSE,&(MVP[0][0]));

		GLuint Max_id = glGetUniformLocation(programID3,"maxpos"); // pobierz lokalizację zmiennej 'uniform' "MV" w programie
		glUniform3f(Max_id,x_axis+translation_x,y_axis+translation_y,z_axis+translation_z);

		glBindVertexArray(VAO[0]);
		glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
		glDrawArrays(GL_TRIANGLES,0,size);
	}

	glFlush();
	glutSwapBuffers();

}
/*###############################################################*/
void rozmiar(int width, int height)
{
	screen_width = width;
	screen_height = height;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, screen_width, screen_height);

	P = glm::perspective(glm::radians(60.0f), (GLfloat)screen_width / (GLfloat)screen_height, 1.0f, 1000.0f);

	glutPostRedisplay(); // Przerysowanie sceny
}

/*###############################################################*/
void idle()
{

	glutPostRedisplay();
}

/*###############################################################*/



/*###############################################################*/
int main(int argc, char **argv)
{
	if(argc < 2) //sprawdzenie argumentów
	{
		printf("Program powinien być uruchomiony z dwoma argumentami: nazwa_pliku.obj [nazwa_tekstury.bmp]");
		return -1;
	}
	char* filename = argv[1];
	char* texturename = argv[2];
	// CZYTANIE Z PLIKU	
	std::vector<Vertex> Object = objload(filename);
	printf("Ilosc wierzcholkow - %d ",Object.size());
	size = Object.size();
	Vertex* vertexArray = &Object[0]; //przekonwertowujemy wektor na tablice Vertex
	//---------------------------------------------------------------------------
	
	// Obliczanie koniecznych przesunięć i rozmiarów osi
	translation_x = Object[0].position.x;
	translation_y = Object[0].position.y;
	translation_z = Object[0].position.z;
	x_axis = Object[0].position.x;
	y_axis = Object[0].position.y;
	z_axis = Object[0].position.z;
	for(int i = 0; i < size;i++)
	{
		if(Object[i].position.x<translation_x) translation_x = Object[i].position.x;
		if(Object[i].position.y<translation_y) translation_y = Object[i].position.y;
		if(Object[i].position.z<translation_z) translation_z = Object[i].position.z;

		if(Object[i].position.x>x_axis) x_axis = Object[i].position.x;
		if(Object[i].position.y>y_axis) y_axis = Object[i].position.y;
		if(Object[i].position.z>z_axis) z_axis = Object[i].position.z;
	}

	x_axis = x_axis - translation_x;
	y_axis = y_axis - translation_y;
	z_axis = z_axis - translation_z;
	printf("\nRozmiar osi %f %f %f\n",x_axis,y_axis,z_axis);
	// ustawienia kamery
	if(z_axis > x_axis)
	{
		kameraD = -z_axis*1.7;
	}
	else
	{
		kameraD = -x_axis*1.7;
	}
	translation_vertical = -y_axis*0.3;

	// Obliczenia w celu ustalenia siatki
	x_step = CountStep(x_axis);
	y_step = CountStep(y_axis);
	z_step = CountStep(z_axis);
	printf("Krok na osiach: %f %f %f\n",x_step,y_step,z_step);
	//---------------------------------------------------------------------------
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(screen_width, screen_height);
	glutInitWindowPosition(0, 0);
	glutCreateWindow("Przeglądarka plików obj");
	glewInit(); //init rozszerzeszeń OpenGL z biblioteki GLEW

	glutDisplayFunc(rysuj);			// def. funkcji rysuj¦cej
    glutIdleFunc(idle);			// def. funkcji rysuj¦cej w czasie wolnym procesoora (w efekcie: ci¦gle wykonywanej)
	glutReshapeFunc(rozmiar); // def. obs-ugi zdarzenia resize (GLUT)
									
	glutKeyboardFunc(klawisz);		// def. obsługi klawiatury
	glutSpecialFunc(specialKeys);
	glutMouseFunc(mysz); 		// def. obsługi zdarzenia przycisku myszy (GLUT)
	glutMotionFunc(mysz_ruch); // def. obsługi zdarzenia ruchu myszy (GLUT)


	glEnable(GL_DEPTH_TEST);

	glGenVertexArrays(1,VAO);

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, size * sizeof(Vertex), vertexArray, GL_STATIC_DRAW);



	glBindVertexArray(VAO[0]);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex,position));
	
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1,2,GL_FLOAT,GL_FALSE,sizeof(Vertex),(GLvoid*)offsetof(Vertex,texcoord));
	
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2,3,GL_FLOAT,GL_FALSE,sizeof(Vertex),(GLvoid*)offsetof(Vertex,normal));
	

	programID = loadShaders("vertex_shader.glsl", "fragment_shader.glsl");
	programID2 = loadShaders("vertex_shader.glsl","fragment_shader2.glsl");
	programID3 = loadShaders("vertex_shader.glsl","fragment_shader3.glsl");

	if(argc == 3)
	{
		tex_id0 = WczytajTeksture(texturename);

		if(tex_id0 == -1)
		{
			MessageBox(NULL,"Nie znaleziono pliku z teksturą","Problem",MB_OK | MB_ICONERROR);
			exit(0);
		}
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D,tex_id0);
		uniformTex0 = glGetUniformLocation(programID,"tex0");
		glUniform1i(uniformTex0,0);
	}

	glutMainLoop();					
	
	glDeleteBuffers(1,&VBO);
	glDeleteBuffers(1,VAO);

	return(0);
}

