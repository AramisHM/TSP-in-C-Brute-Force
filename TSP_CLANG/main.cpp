/*  Travelling Salesman Problem in C-Lang
	Brute Force Implementation - Aramis Hornung Moraes (c) 2017
	Released under the MIT License
*/

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*Returns the size of a given file*/
long ahmlu_get_file_size(char* file_path)
{
	FILE *fp;
	long file_size;

	fp = fopen (file_path , "rb");

	if(!fp)
	{
		printf("Can't find or open the file \"%s\" for reading.\n", file_path);
		return 0;
	}

	// Get the file size in bytes.
	fseek(fp, 0L, SEEK_END);
	file_size = ftell(fp);
	rewind(fp);

	fclose (fp);

	return file_size;
}

/*Returns a char array formated data from a file
WARNING !not null terminated!!!
DO NOT FORGET TO FREE THE RETURN OF THIS FUNCTION!!!
returns 0 if fails*/
unsigned char *ahmlu_create_char_array_from_file(char* file_path)
{
	unsigned char *ret; /* we will store the file data here */
	long fsize; /* the file size */
	FILE * pFile; /* file pointer */

	fsize = ahmlu_get_file_size(file_path);
	ret = (unsigned char*)malloc(fsize*sizeof(unsigned char)); /* allocate needed memory */

	pFile = fopen (file_path, "rb");
  	if (!pFile)
	{

		printf("Can't find or open the file \"%s\" for reading.\n", file_path);
		fputs ("File error",stderr);
		return 0;
	}
	fread (ret,1,fsize,pFile);
	fclose(pFile);

	return ret;
}

/*	loads all cities from the given file to parse into the cities array
	returns the number of cities loaded */
int loadGraph(char* file_to_parse, char **city_names, int *distances) {
	char *raw_file;
	long fs = ahmlu_get_file_size(file_to_parse);
	raw_file = (char*)ahmlu_create_char_array_from_file(file_to_parse);

	/* get cities names */
	char *cities = strtok(raw_file,"\n"); /*first line*/
	char *city_name = strtok(cities,"\",");
	
	int city_count = 1;

	while (city_name) {
		city_name = strtok(NULL,"\",");
		if (city_name &&
			city_name[0] != ',' &&
			city_name[0] != '\r')
			++city_count;
	}

	city_names = (char**)malloc(sizeof(char*)*city_count);
	free(raw_file);

	raw_file = (char*)ahmlu_create_char_array_from_file(file_to_parse);

	int aux= 0;
	cities = strtok(raw_file,"\n"); /*first line*/
	city_name = strtok(cities,"\",");
	while(city_name) {
		
		if (city_name &&
			city_name[0] != ',' &&
			city_name[0] != '\r') {
			city_names[aux] = (char*)malloc(sizeof(char)*(strlen(city_name)+1));
			memccpy(city_names[aux], city_name, 1, strlen(city_name));
			city_names[aux][strlen(city_name)]=0;
			++aux;
		}
		city_name = strtok(NULL,"\",");
	}

	free(raw_file);
	raw_file = (char*)ahmlu_create_char_array_from_file(file_to_parse);

	/* debug: print city names */
	for(int i =0; i < city_count; ++i)
		printf("%s\n",city_names[i]);
	
	distances = (int*)malloc(sizeof(int)*(city_count*city_count));

	strtok(raw_file,"\n\r"); /*ignore first line*/
	char *distance = strtok(NULL,"\n\r, ");
	aux = 0;
	while (distance) {
		distances[aux++] = atoi(distance);
		distance = strtok(NULL,"\n\r, ");
		printf("%d\n",distances[aux-1]);
	}
	free(raw_file);

	return city_count;
}

void generate_routes(int count_left, int current_visited_index, int *path, int *visited_cities) {
	/* no cities left, lets determinate the path value*/
	if (!count_left) {
		for(int i =0; i < current_visited_index; ++ i)
			printf("%d,", path[i]);
		printf("\n");
	} else {
		for (int i = current_visited_index; i < (current_visited_index + count_left); ++i) {
			if (!visited_cities[i]) {
				visited_cities[i] = 1;
				path[current_visited_index]=i;
				generate_routes(count_left-1,current_visited_index+1,path,visited_cities);
				visited_cities[i] = 0;
			}
		}
	}
}


void tsp_brute_force() {

	/* print upper matrix to write the possible paths */
	int n_cities = 4;
	for (int i = 0; i < n_cities; ++i)
		for (int j = i; j < n_cities; ++j)
			if (i!=j)
				printf("Path from city %d to city %d\n", i, j);
}

int main() {
	int *distances = NULL;
	char **city_names = NULL;
	int n_cities = loadGraph("arquivolixo.csv", city_names, distances);
	int path[4];
	int visited_cities[4]= {0,0,0,0};
	generate_routes(n_cities,0,path,visited_cities);


}

#ifdef __cplusplus
}
#endif