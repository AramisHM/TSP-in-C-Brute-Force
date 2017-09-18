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
long ahmlu_get_file_size(char* file_path) {
	FILE *fp;
	long file_size;

	fp = fopen (file_path , "rb");

	if(!fp) {
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
unsigned char *ahmlu_create_char_array_from_file(char* file_path) {
	unsigned char *ret; /* we will store the file data here */
	long fsize; /* the file size */
	FILE * pFile; /* file pointer */

	fsize = ahmlu_get_file_size(file_path);
	ret = (unsigned char*)malloc(fsize*sizeof(unsigned char)); /* allocate needed memory */

	pFile = fopen (file_path, "rb");
  	if (!pFile) {
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
int get_n_cities(char* file_to_parse) {
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
	free(raw_file);
	return city_count;
}

/* parse file and get the city names*/
char **get_city_names(char *file_to_parse, int city_count) {
	char *raw_file;
	char **city_names = (char**)malloc(sizeof(char*)*city_count);
	
	raw_file = (char*)ahmlu_create_char_array_from_file(file_to_parse);

	int aux= 0;
	char *cities = strtok(raw_file,"\n"); /*first line*/
	char *city_name = strtok(cities,"\",");
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
	/* debug: print city names */
	for(int i =0; i < city_count; ++i)
		printf("%s\n",city_names[i]);
	
	return city_names;
}

int *get_distances_vecotr(char* file_to_parse, int city_count) {
	char *raw_file;
	
	int *distances = NULL;

	raw_file = (char*)ahmlu_create_char_array_from_file(file_to_parse);

	distances = (int*)malloc(sizeof(int)*(city_count*city_count));

	strtok(raw_file,"\n\r"); /*ignore first line*/
	char *distance = strtok(NULL,"\n\r, ");
	int aux = 0;
	while (distance) {
		distances[aux++] = atoi(distance);
		distance = strtok(NULL,"\n\r, ");
		//printf("%d ",distances[aux-1]);
	}
	free(raw_file);

	for(int i = 0; i < (city_count*city_count); ++i)
		printf("%d ", distances[i]);
	return distances;
}

int get_cost(int* values, int n_cities, int city_a, int city_b) {
	return values[((n_cities*city_b)+city_a)];
}

unsigned long factorial(unsigned long f)
{
    if ( f == 0 ) 
        return 1;
    return(f * factorial(f - 1));
}

void swap(int *a, int p1, int p2) {
	int aux = a[p1];
	a[p1] = a[p2];
	a[p2] = aux;
}

/* source about vector permutation: https://goo.gl/c1ey41 */
void permute(int *route, int l, int r, int *storage_counter, int** storage) {
   int i;
   if (l == r) {
	   /*
	   for (int i = 0; i <= l; ++i)
		   printf("%d ", route[i]);
	   printf("\n");
	   */
	   for (int i = 0; i <= l; ++i)
		   storage[(*storage_counter)][i] = route[i];
	   ++(*storage_counter);

   } else {
       for (i = l; i <= r; ++i) {
          swap(route, l, i);	  
          permute(route, l+1, r, storage_counter, storage);
          swap(route, l, i); //backtrack
       }
   }
}

/* simply allocates the vector and initialize it with city's id for permutation */
int *allocate_city_vector(unsigned int n_cities) {
	int *ret = (int*)malloc(sizeof(int)*n_cities);

	for(int i = 0; i < n_cities; ++ i) {
		ret[i] = i;
	}
	return ret;
}

int **allocate_city_paths_vector(unsigned n_cities) {
	long n_routs = factorial(n_cities);
	int **paths = (int**)(malloc(sizeof(int*)*n_routs));

	for ( int i = 0; i < n_routs; ++i) {
		int *path = (int*)(malloc(sizeof(int)*n_cities));
		paths[i] = path;
	}
	return paths;
}

void free_city_paths_vector(int **paths, int n_cities) {
	long n_routs = factorial(n_cities);
	
	for ( int i = 0; i < n_routs; ++i)
		free(paths[i]);
	free(paths);
}

int main(int argc, char *argv[]) {

	if(argc < 3)
		printf("Usage: (string) FILE.CSV, (integer) city to start traveling\n");

	int *distances = NULL; /* a array that represents the distances between cities, serialized form */
	char **city_names = NULL; /* array with city names*/

	/* loads the city names and returns the number of cities loaded*/
	int n_cities = get_n_cities(argv[1]);
	city_names = get_city_names(argv[1], n_cities);
	/* gets the cities costs for travelling */
	distances = get_distances_vecotr(argv[1],n_cities);

	/* gets the quantity of possible travelling permutation */
	int n_paths_permutation = factorial(n_cities);

	int *city_ids_for_permutation = allocate_city_vector(n_cities);
	
	/* array with travelling paths */
	int **paths = allocate_city_paths_vector(n_cities);
	int n_routes = 0;
	permute(city_ids_for_permutation, 0, n_cities-1, &n_routes, paths);
	
	printf("\n\npaths and costs\n");
	
	int start_city = atoi(argv[2]);
	int cheapest_path = INT_MAX;
	int index_of_cheapest_path = 0;

	for(int i = 0; i < n_paths_permutation; ++i) {
		if(paths[i][0] != start_city) /* we only want paths that start in a particular city */
			continue;
		int cost_sum = 0;
		for(int j = 0; j < n_cities; ++j) {
			printf("%d ", paths[i][j]);
			if(j!=0)
				cost_sum += get_cost(distances,n_cities, paths[i][j-1], paths[i][j]);	
		}
		printf(" - %d ", cost_sum);

		if(cheapest_path > cost_sum) {
			printf(" shorter than previous ");
			cheapest_path = cost_sum; /* the path actual total cost*/
			index_of_cheapest_path = i; /* the path index in the paths array*/
		}

		printf("\n");
	}

	printf("\n\n--------------------------------\n\nRESULTS\n\n");

	printf("Selected city as start point is \"%s\" (id:%d)\n",city_names[start_city],start_city);
	printf("Shortest rout is:\n");
	for(int j = 0; j < n_cities; ++j) {
		int city_index = paths[index_of_cheapest_path][j];
		printf("%s-(%d)%s ", city_names[city_index],\
							 (j+1!=n_cities? get_cost(distances,n_cities, paths[index_of_cheapest_path][j], paths[index_of_cheapest_path][j+1]):0),\
							 (j<n_cities-1?"->":""));
	}
	printf("(costing a total of %d)\n", cheapest_path);
	
	free_city_paths_vector(paths, n_cities);
	free(city_ids_for_permutation);
	free(distances);
	free(city_names);
}

#ifdef __cplusplus
}
#endif