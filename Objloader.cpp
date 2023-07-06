#include "Objloader.h"

//funkcja wczytuj¹ca obj
std::vector<Vertex> objload(const char* file_name)
{
	std::vector<Vertex> res; //wyktor zwracany
	std::string prefix;

	//wspó³rzêdne
	std::vector<glm::vec3> vertices;
	glm::vec3 temp_vertices;

	//koordynaty tekstury
	std::vector<glm::vec2> texCoord;
	glm::vec2 temp_texCoord;

	//wektory normalne
	std::vector<glm::vec3> normalVec;
	glm::vec3 temp_normalVec;

	//numery danych
	std::vector<GLuint> vertice_index;
	std::vector<GLuint> texCoord_index;
	std::vector<GLuint> normalVec_index;
	GLuint temp_index;

	std::ifstream in{file_name}; //otwieramy plik
	std::string s; //bedziemy tu wczytywaæ wiersze 


	while(getline(in,s)) {
		std::stringstream str(s); //otwieramy strumieñ stringów
		str >> prefix; //wczytujemy prefiks
		if(prefix == "v") // v oznacza wspó³rzêdne
		{
			//printf("Wierzcholek ");
			str >> temp_vertices.x >> temp_vertices.y >> temp_vertices.z;
			//printf("%f %f %f\n",temp_vertices.x,temp_vertices.y,temp_vertices.z);
			vertices.push_back(temp_vertices);
		}
		else if(prefix == "vt") //vt oznacza koordynaty tekstury
		{
			//printf("Tekstura ");
			str >> temp_texCoord.x >> temp_texCoord.y;
			//printf("%f %f\n",temp_texCoord.x,temp_texCoord.y);
			texCoord.push_back(temp_texCoord);
		}
		else if(prefix == "vn") //vn oznacza wektor normalny
		{
			//printf("Wektor normalny ");
			str >> temp_normalVec.x >> temp_normalVec.y >> temp_normalVec.z;
			//printf("%f %f %f\n",temp_normalVec.x,temp_normalVec.y,temp_normalVec.z);
			normalVec.push_back(temp_normalVec);
		}
		else if(prefix == "f") //f kolejnoœæ
		{
			//printf("\nEBOOO");

			for(int i = 1;i<4;i++)
			{
				str >> temp_index;
				//printf(" %d ",temp_index);
				vertice_index.push_back(temp_index);
				str.get();
				str >> temp_index;
				//printf(" %d ",temp_index);
				texCoord_index.push_back(temp_index);
				str.get();
				str >> temp_index;
				//printf(" %d ",temp_index);
				normalVec_index.push_back(temp_index);
			}
		}
	}
	//te rozmiary tak w³aœciwie powinny byæ takie same
	GLuint size_vertices = vertice_index.size();
	GLuint size_texCoord = texCoord_index.size();
	GLuint size_normalVec = normalVec_index.size();
	//printf("\nsizes: %d %d %d\n",size_vertices,size_texCoord,size_normalVec);

	res.resize(size_vertices,Vertex()); // rezerwujemy miejsce w wektorze na wszystkie potrzebne dane
	//uzupe³niamy nasz wektor wynikowy
	for(int i = 0;i<size_vertices;i++)
	{
		res[i].position = vertices[vertice_index[i]-1];
		res[i].texcoord = texCoord[texCoord_index[i]-1];
		res[i].normal = normalVec[normalVec_index[i]-1];
	}

	return res;
}