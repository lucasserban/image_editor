#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define SIZE_OF_COMMAND 100

typedef struct {
	char format[5];
	int height;
	int width;
	int max_value;
} img;

int count_digits(int x);
void print_matrix(unsigned char **a, int x, int y);
void image_info(FILE *image, img *data);
void free_matrix(unsigned char **a, int height);
void allocate_matrix(unsigned char **a, int height, int width);
void load(FILE **org_image, unsigned char ***image, unsigned char ***aux, int *select_height, int *select_width,
char *command, img *data, int *cnt_load, int *is_loaded, int *image_height, int *image_width,
int *max_x, int *max_y, int *min_x, int *min_y, int *cnt_equalize);
void apply(FILE *org_image, char *command, img *data, int image_height, int image_width, unsigned char **image,
unsigned char **aux, int min_x, int min_y, int max_x, int max_y);
void histogram(FILE *org_image, char *command, img data, int image_height, int image_width, unsigned char **image);
void equalize(FILE *org_image, img data, int *cnt_equalize, int image_height, int image_width, unsigned char **aux,
unsigned char **image);
void select(FILE *org_image, char *command, int x1, int x2, int y1, int y2, int *min_x, int *min_y,
int *max_x, int *max_y, img data, int image_height, int image_width, int *select_width, int *select_height);
void select_all(FILE *org_image, img data, int image_height, int image_width, int *select_height, int *select_width, int *min_x,
int *min_y, int *max_x, int *max_y);
void save(FILE *org_image, img data, char *command, int image_width, int image_height, unsigned char **image);
void crop(FILE *org_image, int *image_height, int *image_width, int select_height, int select_width,
int *min_x, int* min_y, int *max_x, int *max_y, img data, unsigned char ***image, unsigned char ***aux, int *cnt_equalize);
void rotate(FILE *org_image, char *command, int *min_x, int *min_y, int *max_x, int *max_y, int *image_width, int *image_height,
img *data, unsigned char ***image, unsigned char ***aux);

int main(void)
{
	//initializez variabilele de care am nevoie tot programul
	char command[SIZE_OF_COMMAND];
	memset(command, 0, 100);
	FILE *org_image = NULL;
	img data = {"PO", 0, 0, 0};
	unsigned char **image = NULL, **aux = NULL;
	int select_height = 0, select_width = 0, image_height = 0, image_width = 0,
	is_loaded = 0, cnt_load = 0, cnt_equalize = 0;
	int x1 = 0, y1 = 0, x2 = 0, y2 = 0, min_x = 0, min_y = 0, max_x = 0, max_y = 0;
	memset(data.format, 0, 5);

	while (strncmp("EXIT", command, 4) != 0) {
		fgets(command, SIZE_OF_COMMAND - 1, stdin);
		if (strncmp("LOAD", command, 4) == 0) {
			load(&org_image, &image, &aux, &select_height, &select_width, command, &data, &cnt_load,
			&is_loaded, &image_height, &image_width, &max_x, &max_y, &min_x, &min_y, &cnt_equalize);
		} else if (strncmp("SELECT ALL", command, 10) == 0) {
			select_all(org_image, data, image_height, image_width, &select_height, &select_width, &min_x,
			&min_y, &max_x, &max_y);
		} else if (strncmp("SELECT", command, 6) == 0) {
			select(org_image, command, x1, x2, y1, y2, &min_x, &min_y, &max_x, &max_y, data, image_height,
			image_width, &select_width, &select_height);
		} else if (strncmp("CROP", command, 4) == 0) {
			crop(org_image, &image_height, &image_width, select_height, select_width, &min_x,
			&min_y, &max_x, &max_y, data, &image, &aux, &cnt_equalize);
		} else if (strncmp("SAVE", command, 4) == 0) {
			save(org_image, data, command, image_width, image_height, image);
		} else if (strncmp("APPLY", command, 5) == 0) {
			apply(org_image, command, &data, image_height, image_width, image, aux, min_x, min_y, max_x, max_y);
		} else if (strncmp("HISTOGRAM", command, 9) == 0) {
			histogram(org_image, command, data, image_height, image_width, image);
		} else if (strncmp("EQUALIZE", command, 8) == 0) {
			equalize(org_image, data, &cnt_equalize, image_height, image_width, aux, image);
		} else if (strncmp("ROTATE", command, 6) == 0) {
			rotate(org_image, command, &min_x, &min_y, &max_x, &max_y, &image_width, &image_height, &data, &image, &aux);
		} else if (strncmp("EXIT",  command, 4) != 0) {
			printf("Invalid command\n");
		}
	}
	//Eliberez memoria alocata prin comanda EXIT
	if (!org_image)
		printf("No image loaded\n");
	free_matrix(image, image_height);
	free_matrix(aux, image_height);
	if (org_image)
		fclose(org_image);
    return 0;
}

void load(FILE **org_image, unsigned char ***image, unsigned char ***aux, int *select_height, int *select_width,
char *command, img *data, int *cnt_load, int *is_loaded, int *image_height, int *image_width,
int *max_x, int *max_y, int *min_x, int *min_y, int *cnt_equalize)
{
   //In caz ca avem deja un fisier incarcat
	if (*org_image) {
		fclose(*org_image);
	}
	//aloc dinamic numele fisierului
	char filename[SIZE_OF_COMMAND];
	strcpy(filename, command + 5);
	//verific daca fisierul este binar sau ASCII
	*(strchr(filename, '\n')) = '\0';
	*org_image = fopen(filename, "rt");
	if (!(*org_image)) {
		printf("Failed to load %s\n", filename);
		return;
	} else {
		printf("Loaded %s\n", filename);
		//memorez datele imaginii in structura img
		image_info(*org_image, data);
		if (*cnt_load == 0) {
			*select_height = data->height;
			*select_width = data->width;
		}
		if (*is_loaded) {
			free_matrix(*image, *image_height);
			free_matrix(*aux, *image_height);
		}
		//pentru fiecare tip de fisier si de poza(alb-negru sau color) extrag datele din fisier
		*image = (unsigned char **)malloc(data->height * sizeof(unsigned char *));
		if (strncmp("P5", data->format, 2) == 0) {
			for (int i = 0; i < data->height; i++) {
				(*image)[i] = (unsigned char *)malloc(data->width * sizeof(unsigned char));
			}
			*image_width = data->width;
			fclose(*org_image);
			*org_image  = fopen(filename, "rb");
			char aux[5];
			int x;
			char aux2;
			fscanf(*org_image, "%s", aux);
			for (int i = 0 ; i < 3; i++) {
				fscanf(*org_image, "%d", &x);
			}
			fscanf(*org_image, "%c", &aux2);
			for (int i = 0; i < data->height; i++) {
				for (int j = 0; j < data->width; j++) {
					fread(&(*image)[i][j], sizeof(unsigned char), 1, *org_image);
				}
			}
		} else if (strncmp("P6", data->format, 2) == 0) {
			for (int i = 0; i < data->height; i++) {
				(*image)[i] = (unsigned char *)malloc(data->width * 3 * sizeof(unsigned char));
			}
			*image_width = 3 * data->width;
			fclose(*org_image);
			*org_image  = fopen(filename, "rb");
			char aux[5];
			int x;
			char aux2;
			fscanf(*org_image, "%s", aux);
			for (int i = 0 ; i < 3; i++) {
				fscanf(*org_image, "%d", &x);
			}
			fscanf(*org_image, "%c", &aux2);
			for (int i = 0; i < data->height; i++) {
				for (int j = 0; j < 3 * data->width; j++) {
					fread(&(*image)[i][j], sizeof(unsigned char), 1, *org_image);
				}
			}
		} else if (strncmp("P2", data->format, 2) == 0) {
			for (int i = 0; i < data->height; i++) {
				(*image)[i] = (unsigned char *)malloc(data->width * sizeof(unsigned char));
			}
			*image_width = data->width;
			for (int i = 0; i < data->height; i++) {
				for (int j = 0; j < data->width; j++) {
					fscanf(*org_image, "%hhu", &(*image)[i][j]);
				}
			}
		} else if (strncmp("P3", data->format, 2) == 0) {
			for (int i = 0; i < data->height; i++) {
				(*image)[i] = (unsigned char *)malloc(data->width * 3 * sizeof(unsigned char));
			}
			*image_width = 3 * data->width;
			for (int i = 0; i < data->height; i++) {
				for (int j = 0; j < 3 * data->width; j++) {
					fscanf(*org_image, "%hhu", &(*image)[i][j]);
				}
			}
		}
		*is_loaded = 1;
		(*cnt_load)++;
		*image_height = data->height;
		//copiez imaginea in matricea auxiliara
		*aux = (unsigned char **)malloc(data->height * sizeof(unsigned char *));
		*select_height = data->height;
		if (strncmp("P2", data->format, 2) == 0 || strncmp("P5", data->format, 2) == 0) {
			*select_width = data->width;
			*max_x = *select_width;
			for (int i = 0; i < data->height; i++) {
				(*aux)[i] = (unsigned char *)malloc(data->width * sizeof(unsigned char));
			}
			for (int i = 0; i < data->height; i++) {
				for (int j = 0; j < data->width; j++) {
					(*aux)[i][j] = (*image)[i][j];
				}
			}
		} else if (strncmp("P3", data->format, 2) == 0 || strncmp("P6", data->format, 2) == 0) {
			*select_width = data->width * 3;
			*max_x = *select_width / 3;
			for (int i = 0; i < data->height; i++) {
				(*aux)[i] = (unsigned char *)malloc(3 * data->width * sizeof(unsigned char));
			}
			for (int i = 0; i < data->height; i++) {
				for (int j = 0; j < 3 * data->width; j++) {
					(*aux)[i][j] = (*image)[i][j];
				}
			}
		}
		*min_x = 0, *min_y = 0, *max_y = *select_height, *cnt_equalize = 0;
	}
}

void select_all(FILE *org_image, img data, int image_height, int image_width, int *select_height, int *select_width, int *min_x,
int *min_y, int *max_x, int *max_y)
{
	//verific daca fisierul exista
	if (!org_image) {
		printf("No image loaded\n");
		return;
	}
	/*pentru selectare folosesc ca latime si lungime select height
	respectiv select width, iar pentru selectarea totala o setez
	la lungimea si latimea imaginii*/
	*select_height = image_height;
	if (strncmp("P2", data.format, 2) == 0 || strncmp("P5", data.format, 2) == 0) {	
		*select_width = image_width;
		*max_x = *select_width;
	} else if (strncmp("P3", data.format, 2) == 0 || strncmp("P6", data.format, 2) == 0) {
		*select_width = image_width;
		*max_x = *select_width / 3;
	}
	/*setez coordonatele absolute a matricii de selectare*/
	*min_x = 0, *min_y = 0, *max_y = *select_height;
	printf("Selected ALL\n");
}

void select(FILE *org_image, char *command, int x1, int x2, int y1, int y2, int *min_x, int *min_y,
int *max_x, int *max_y, img data, int image_height, int image_width, int *select_width, int *select_height)
{
	if (!org_image) {
		printf("No image loaded\n");
		return;
	}

	int length_command = 10;
	//convertesc x1, x2, y1, y2 din string in numere
	int aux_x1 = atoi(command + 7);
	length_command += count_digits(aux_x1);
	/*aceasta variabila este pentru a sti cate cifra are x1
	pentru a-l putea extrage corect pe y1, y2, si x2 din string*/
	int position = count_digits(aux_x1) + 8;
	if (aux_x1 < 0)
		position++;
	int aux_y1 = atoi(command + position);
	length_command += count_digits(aux_y1);
	position += count_digits(aux_y1) + 1;
	if (aux_y1 < 0)
		position++;
	int aux_x2 = atoi(command + position);
	length_command += count_digits(aux_x2);
	position += count_digits(aux_x2) + 1;
	if (aux_x2 < 0)
		position++;
	int aux_y2 = atoi(command + position);
	length_command += count_digits(aux_y2);

	int aux_max_x = (aux_x1 > aux_x2) ? aux_x1 : aux_x2;
	int aux_max_y = (aux_y1 > aux_y2) ? aux_y1 : aux_y2;

	/*daca vreo variabila din comanda e negativa, inseamna ca 
	lungimea comenzii este cu unu mai mare*/
	if (aux_x1 < 0)
		length_command++;
	if (aux_x2 < 0)
		length_command++;
	if (aux_y1 < 0)
		length_command++;
	if (aux_y2 < 0)
		length_command++;

	if ((int)strlen(command) - 1 != length_command) {
		printf("Invalid command\n");
		return;
	}

	if (strncmp("P2", data.format, 2) == 0 || strncmp("P5", data.format, 2) == 0) {
		if (aux_max_x > image_width || aux_max_y > image_height || aux_x1 < 0 || aux_x2 < 0 || aux_y1 < 0 || aux_y2 < 0 || aux_x1 == aux_x2 || aux_y1 == aux_y2) {
			printf("Invalid set of coordinates\n");
			return;
		}
	} else {
		if (aux_max_x > image_width / 3 || aux_max_y > image_height || aux_x1 < 0 || aux_x2 < 0 || aux_y1 < 0 || aux_y2 < 0 || aux_x1 == aux_x2 || aux_y1 == aux_y2) {
			printf("Invalid set of coordinates\n");
			return;
		}
	}
	x1 = aux_x1;
	x2 = aux_x2;
	y1 = aux_y1;
	y2 = aux_y2;
	*max_x = aux_max_x;
	*max_y = aux_max_y;
	*min_x = (x1 > x2) ? x2 : x1;
	*min_y = (y1 > y2) ? y2 : y1;
	if (strncmp("P2", data.format, 2) == 0 || strncmp("P5", data.format, 2) == 0) {
		*select_width = *max_x - *min_x;
	} else if (strncmp("P3", data.format, 2) == 0 || strncmp("P6", data.format, 2) == 0) {
		*select_width = 3 * (*max_x - *min_x);
	}

	printf("Selected %d %d %d %d\n", *min_x, *min_y, *max_x, *max_y);
	*select_height = *max_y - *min_y;
}

void crop(FILE *org_image, int *image_height, int *image_width, int select_height, int select_width,
int *min_x, int* min_y, int *max_x, int *max_y, img data, unsigned char ***image, unsigned char ***aux, int *cnt_equalize)
{
	if (!org_image) {
		printf("No image loaded\n");
		return;
	}
	//eliberez memoria alocata imaginii
	free_matrix(*image, *image_height);
	*image = (unsigned char **)malloc(select_height * sizeof(unsigned char *));
		for (int i = 0; i < select_height; i++) {
			(*image)[i] = (unsigned char *)malloc(select_width * sizeof(unsigned char));
		}
	//ma folosesc de matricea aux pentru a crea noua imagine cu dimensiuni diferite
	if (strncmp("P2", data.format, 2) == 0 || strncmp("P5", data.format, 2) == 0) {
		for (int i = 0, m = *min_y; i < select_height && m < *max_y; i++, m++) {
			for (int j = 0, n = *min_x; j < select_width && n < *max_x; j++, n++) {
				(*image)[i][j] = (*aux)[m][n];
			}
		}
	} else {
		for (int i = 0, m = *min_y; i < select_height && m < *max_y; i++, m++) {
			for (int j = 0, n = *min_x * 3; j < select_width && n < *max_x * 3; j++, n++) {
				(*image)[i][j] = (*aux)[m][n];
			}
		}
	}
	//acum eliberez matricea aux
	free_matrix(*aux, *image_height);
	*aux = (unsigned char **)malloc(select_height * sizeof(unsigned char *));
		for (int i = 0; i < select_height; i++) {
			(*aux)[i] = (unsigned char *)malloc(select_width * sizeof(unsigned char));
		}
	//ma folosesc de noua imagine pentru a purta mereu o copie a imaginii in program
	for (int i = 0; i < select_height; i++) {
		for (int j = 0; j < select_width; j++) {
			(*aux)[i][j] = (*image)[i][j];
		}
	}
	if (strncmp("P2", data.format, 2) == 0 || strncmp("P5", data.format, 2) == 0) {
		*max_x = select_width;
	} else {
		*max_x = select_width / 3;
	}
	//noile valori ale imaginii
	*image_height = select_height;
	*image_width = select_width;
	*min_y = 0, *min_x = 0, *max_y = *image_height, *cnt_equalize = 0;
	printf("Image cropped\n");
}

void save(FILE *org_image, img data, char *command, int image_width, int image_height, unsigned char **image)
{
	if (!org_image) {
		printf("No image loaded\n");
		return;
	}
	//aloc dinamic numele fisierului salvat
	char filename_saved[SIZE_OF_COMMAND];
	strcpy(filename_saved, command + 5);
	if (strrchr(filename_saved, ' '))
		*(strrchr(filename_saved, ' ')) = '\0';
	char *file_type = strstr(command, "ascii");
	if (!file_type) {
		*(strrchr(filename_saved, '\n')) = '\0';
		FILE *save = fopen(filename_saved, "wb");
		if (strncmp("P2", data.format, 2) == 0 || strncmp("P5", data.format, 2) == 0) {
			fprintf(save, "%s\n%d %d\n%d\n", "P5", image_width, image_height, data.max_value);
			for (int i = 0; i < image_height; i++) {
				for (int j = 0; j < image_width; j++) {
					fwrite(&image[i][j], sizeof(unsigned char), 1, save);
				}
			}
		} else if (strncmp("P3", data.format, 2) == 0 || strncmp("P6", data.format, 2) == 0) {
			fprintf(save, "%s\n%d %d\n%d\n", "P6", image_width / 3, image_height, data.max_value);
			for (int i = 0; i < image_height; i++) {
				for (int j = 0; j < image_width; j++) {
					fwrite(&image[i][j], sizeof(unsigned char), 1, save);
				}
			}
		}
		printf("Saved %s\n", filename_saved);
		fclose(save);
	} else if (strncmp("ascii", file_type, 5) == 0) {
		FILE *save = fopen(filename_saved, "wt");
		if (strncmp("P2", data.format, 2) == 0 || strncmp("P5", data.format, 2) == 0) {
			fprintf(save, "%s\n%d %d\n%d\n", "P2", image_width, image_height, data.max_value);
		} else if (strncmp("P3", data.format, 2) == 0 || strncmp("P6", data.format, 2) == 0) {
			fprintf(save, "%s\n%d %d\n%d\n", "P3", image_width / 3, image_height, data.max_value);
		}
		for (int i = 0; i < image_height; i++) {
			for (int j = 0; j < image_width; j++) {
				fprintf(save, "%hhu ", image[i][j]);
			}
			fprintf(save, "\n");
		}
		printf("Saved %s\n", filename_saved);
		fclose(save);
	}
}

void histogram(FILE *org_image, char *command, img data, int image_height, int image_width, unsigned char **image)
{
	if (!org_image) {
		printf("No image loaded\n");
		return;
	}
	if (*(command + 10) == '\0') {
		printf("Invalid command\n");
		return;
	}
	if (strncmp("P3", data.format, 2) == 0 || strncmp("P6", data.format, 2) == 0) {
		printf("Black and white image needed\n");
		return;
	}
	int command_length = 11;
	int max_stars = atoi(command + 10);
	command_length += count_digits(max_stars);
	int bins = atoi(command + 10 + count_digits(max_stars));
	command_length += count_digits(bins);
	if (bins <= 0) {
		printf("Invalid command\n");
		return;
	}
	if ((int)strlen(command) - 1 != command_length) {
		printf("Invalid command\n");
		return;
	}
	int max_frequency = 0;
	//calculez care este frecventa maxima
	for (int i = 0; i <= data.max_value; i += 256 / bins) {
		int cnt = 0;
		for (int j = i; j < i + 256 / bins; j++) {
			for (int m = 0; m < image_height; m++) {
				for (int n = 0; n < image_width; n++) {
					if (image[m][n] == j) {
						cnt++;
					}
				}
			}
		}
		if (cnt > max_frequency) {
			max_frequency = cnt;
		}
	}
	for (int i = 0; i <= data.max_value; i += 256 / bins) {
		double cnt = 0;
		for (int j = i; j < i + (256 / bins); j++) {
			for (int m = 0; m < image_height; m++) {
				for (int n = 0; n < image_width; n++) {
					if (image[m][n] == j) {
						cnt++;
					}

				}
			}
		}
		double star_num = floor((cnt / max_frequency) * max_stars);
		printf("%0.lf\t|\t", star_num);
		for (double j = 0; j < star_num; j++) {
			printf("*");
		}
		printf("\n");
	}
}

void apply(FILE *org_image, char *command, img *data, int image_height, int image_width, unsigned char **image,
unsigned char **aux, int min_x, int min_y, int max_x, int max_y)
{
	/*Ideea este aceeasi la toate cele 4 efecte ale apply-ului, verific marginile
	selectiei pentru a vedea daca pot calcula valoarea acelui pixel(daca are toti vecinii)
	si in functie de efect aplic kernelul pe pixeli si le asignez noua valoare*/
	if (!org_image) {
		printf("No image loaded\n");
		return;
	}
	if (*(command + 6) == '\0') {
		printf("Invalid command\n");
		return;
	}
	char parameter[30];
	strcpy(parameter, command + 6);
	*(strrchr(parameter, '\n')) = '\0';
	if (strncmp("SHARPEN", parameter, 7) == 0) {
		if (strncmp("P2", data->format, 2) == 0 || strncmp("P5", data->format, 2) == 0) {
			printf("Easy, Charlie Chaplin\n");
		} else if (strncmp("P3", data->format, 2) == 0 || strncmp("P6", data->format, 2) == 0) {
			for (int i = min_y ; i <  max_y; i++) {
				for (int j = min_x * 3; j < max_x * 3; j++) {
					int sum = 0;
					if (j - 3 >= 0)
						sum += -1 * aux[i][j - 3];
					else
						continue;

					if (i - 1 >= 0)
						sum += -1 * aux[i - 1][j];
					else
						continue;

					if (i + 1 < image_height)
						sum += -1 * aux[i + 1][j];
					else
						continue;
					if (j + 3 < image_width)
						sum += -1 * aux[i][j + 3];
					else
						continue;

					sum += aux[i][j] * 5;

					if (sum < 0)
						sum = 0;
					else if (sum > 255)
						sum = 255;

					image[i][j] = sum;
				}
			}
			printf("APPLY %s done\n", parameter);
			//La fiecare efect actualizez copia imaginii
			for (int i = 0; i < image_height; i++) {
				for (int j = 0; j < image_width; j++) {
					aux[i][j] = image[i][j];
				}
			}
		}
	} else if (strncmp("EDGE", parameter, 4) == 0) {
		if (strncmp("P2", data->format, 2) == 0 || strncmp("P5", data->format, 2) == 0) {
			printf("Easy, Charlie Chaplin\n");
		} else if (strncmp("P3", data->format, 2) == 0 || strncmp("P6", data->format, 2) == 0) {
			for (int i = min_y ; i < max_y; i++) {
				for (int j = min_x * 3; j < max_x * 3; j++) {
					int sum = 0;
					if (j - 3 >= 0)
						sum += -1 * aux[i][j - 3];
					else
						continue;

					if (i - 1 >= 0)
						sum += -1 * aux[i - 1][j];
					else
						continue;

					if (i + 1 < image_height)
						sum += -1 * aux[i + 1][j];
					else
						continue;
					if (j + 3 < image_width)
						sum += -1 * aux[i][j + 3];
					else
						continue;

					sum += aux[i][j] * 8 + (-1 * aux[i - 1][j - 3]) + (-1 * aux[i - 1][j + 3]) + (-1 * aux[i + 1][j - 3]) + (-1 * aux[i + 1][j + 3]);

					if (sum < 0)
						sum = 0;
					else if (sum > 255)
						sum = 255;

					image[i][j] = sum;
				}
			}
			printf("APPLY %s done\n", parameter);
			for (int i = 0; i < image_height; i++) {
				for (int j = 0; j < image_width; j++) {
					aux[i][j] = image[i][j];
				}
			}
		}
	} else if (strncmp("BLUR", parameter, 4) == 0) {
		if (strncmp("P2", data->format, 2) == 0 || strncmp("P5", data->format, 2) == 0) {
			printf("Easy, Charlie Chaplin\n");
		} else if (strncmp("P3", data->format, 2) == 0 || strncmp("P6", data->format, 2) == 0) {
			for (int i = min_y ; i < max_y; i++) {
				for (int j = min_x * 3; j < max_x * 3; j++) {
					double sum = 0;
					if (j - 3 >= 0)
						sum += aux[i][j - 3];
					else
						continue;

					if (i - 1 >= 0)
						sum += aux[i - 1][j];
					else
						continue;

					if (i + 1 < image_height)
						sum += aux[i + 1][j];
					else
						continue;
					
					if (j + 3 < image_width)
						sum += aux[i][j + 3];
					else
						continue;

					sum += aux[i][j] + aux[i - 1][j - 3] + aux[i - 1][j + 3] + aux[i + 1][j - 3] + aux[i + 1][j + 3];
					sum /= 9;
					if (sum < 0)
						sum = 0;
					else if (sum > 255)
						sum = 255;
					image[i][j] = round(sum);
				}
			}
			printf("APPLY %s done\n", parameter);
			for (int i = 0; i < image_height; i++) {
				for (int j = 0; j < image_width; j++) {
					aux[i][j] = image[i][j];
				}
			}
		}
	} else if (strncmp("GAUSSIAN_BLUR", parameter, 13) == 0) {
		if (strncmp("P2", data->format, 2) == 0 || strncmp("P5", data->format, 2) == 0) {
			printf("Easy, Charlie Chaplin\n");
		} else if (strncmp("P3", data->format, 2) == 0 || strncmp("P6", data->format, 2) == 0) {
			for (int i = min_y ; i <  max_y; i++) {
				for (int j = min_x * 3; j < max_x * 3; j++) {
					double sum = 0;
					if (j - 3 >= 0)
						sum += 2 * aux[i][j - 3];
					else
						continue;

					if (i - 1 >= 0)
						sum += 2 * aux[i - 1][j];
					else
						continue;

					if (i + 1 < image_height)
						sum += 2 * aux[i + 1][j];
					else
						continue;
					
					if (j + 3 < image_width)
						sum += 2 * aux[i][j + 3];
					else
						continue;

					sum += aux[i][j] * 4 + aux[i - 1][j - 3] + aux[i - 1][j + 3] + aux[i + 1][j - 3] + aux[i + 1][j + 3];
					sum /= 16;
					if (sum < 0)
						sum = 0;
					else if (sum > 255)
						sum = 255;
					image[i][j] = round(sum);
				}
			}
			printf("APPLY %s done\n", parameter);
			for (int i = 0; i < image_height; i++) {
				for (int j = 0; j < image_width; j++) {
					aux[i][j] = image[i][j];
				}
			}
		}
	} else {
		printf("APPLY parameter invalid\n");
		return;
	}
}

void equalize(FILE *org_image, img data, int *cnt_equalize, int image_height, int image_width, unsigned char **aux,
unsigned char **image)
{
	if (!org_image) {
		printf("No image loaded\n");
		return;
	}
	if (strncmp("P3", data.format, 2) == 0 || strncmp("P6", data.format, 2) == 0) {
		printf("Black and white image needed\n");
		return;
	}
	//creez un vector de frecventa pentru a ma ajuta la calcule
	if (*cnt_equalize == 0) {
		(*cnt_equalize)++;
		int area = image_height * image_width;
		int freq[256] = {0};
			for (int m = 0; m < image_height; m++) {
				for (int n = 0; n < image_width; n++) {
						freq[aux[m][n]]++;
				}
			}

		for (int i = 0; i < image_height; i++) {
			for (int j = 0; j < image_width; j++) {
				int sum = 0;
				for (int k = 0; k <= aux[i][j]; k++) {
					sum += freq[k];
				}
				int new_value = round(255.0 / area * sum);
				if (new_value > 255) {
					new_value = 255;
				} else if (new_value < 0) {
					new_value = 0;
				}
				image[i][j] = new_value;
			}
		}
		for (int i = 0; i < image_height; i++) {
			for (int j = 0; j < image_width; j++) {
				aux[i][j] = image[i][j];
			}
		}
	}
	printf("Equalize done\n");
}

void rotate(FILE *org_image, char *command, int *min_x, int *min_y, int *max_x, int *max_y, int *image_width, int *image_height,
img *data, unsigned char ***image, unsigned char ***aux)
{
	if (!org_image) {
		printf("No image loaded\n");
		return;
	}
	int angle = atoi(command + 7);

	if (angle % 90 != 0) {
		printf("Unsupported rotation angle\n");
		return;
	}

	if (*max_x - *min_x != *max_y - *min_y && *max_x != *image_width && *max_y != *image_height) {
		printf("The selection must be square\n");
		return;
	}

	if (strncmp("P3", data->format, 2) == 0 || strncmp("P6", data->format, 2) == 0) {
		*max_x *= 3;
	}
	/*daca este selectata toata imaginea actualizez noile valori ale imaginii, depinzand
	de unghiul care este rotit in cazul la 180 si 360 de grade doar aplic formula de rotatie,
	iar in cazul selectiilor patrate aplic formulele de rotatie*/
	if (*max_x == *image_width && *max_y == *image_height && *min_x == 0 && *min_y == 0) {
		if (strncmp("P3", data->format, 2) == 0 || strncmp("P6", data->format, 2) == 0) {
			if (angle == 90 || angle == -90 || angle == -270 || angle == 270) {
				free_matrix(*image, *image_height);
				data->height = *image_width / 3;
				data->width = *image_height;
				*image_height = data->height;
				*image_width = data->width * 3;
				*max_x = *image_width / 3;
				*max_y = *image_height;
				*min_x = 0;
				*min_y = 0;
				*image = (unsigned char **)malloc(*image_height * sizeof(unsigned char *));
				for (int i = 0; i < *image_height; i++) {
					(*image)[i] = (unsigned char *)malloc(*image_width * sizeof(unsigned char));
				}
		
				if (angle == -90 || angle == 270) {
					for (int i = *min_y; i < *max_y; i++) {
						for (int j = *min_x; j < *max_x; j++) {
							(*image)[i][3 * j] = (*aux)[j][3 * (*max_y - 1 - i)];
							(*image)[i][3 * j + 1] = (*aux)[j][3 * (*max_y - 1 - i) + 1];
							(*image)[i][3 * j + 2] = (*aux)[j][3 * (*max_y - 1 - i) + 2];
						}
					}
				} else if (angle == 90 || angle == -270) {
					for (int i = *min_y; i < *max_y; i++) {
						for (int j = *min_x; j < *max_x; j++) {
							(*image)[i][3 * j] = (*aux)[*max_x - 1 - j][3 * i];
							(*image)[i][3 * j + 1] = (*aux)[*max_x - 1 - j][3 * i + 1];
							(*image)[i][3 * j + 2] = (*aux)[*max_x - 1 - j][3 * i + 2];
						}
					}
				}
				free_matrix(*aux, data->width);
				*aux = (unsigned char **)malloc(*image_height * sizeof(unsigned char *));
				for (int i = 0; i < *image_height; i++) {
					(*aux)[i] = (unsigned char *)malloc(*image_width * sizeof(unsigned char));
				}
				for (int i = 0; i < *image_height; i++) {
					for (int j = 0; j < *image_width; j++) {
						(*aux)[i][j] = (*image)[i][j];
					}
				}
			} else {
				*max_x /= 3;
				if (angle == 180 || angle == -180) {
					for (int i = *min_y; i < *max_y; i++) {
						for (int j = *min_x; j < *max_x; j++) {
							(*image)[i][3 * j] = (*aux)[*max_y - 1 - i][3 * (*max_x - 1 - j)];
							(*image)[i][3 * j + 1] = (*aux)[*max_y - 1 - i][3 * (*max_x - 1 - j) + 1];
							(*image)[i][3 * j + 2] = (*aux)[*max_y - 1 - i][3 * (*max_x - 1 - j) + 2];
						}
					}
				}
				for (int i = 0; i < *image_height; i++) {
					for (int j = 0; j < *image_width; j++) {
						(*aux)[i][j] = (*image)[i][j];
					}
				}
			}
		} else {
			if (angle == 90 || angle == -90 || angle == -270 || angle == 270) {
				free_matrix(*image, *image_height);
				data->height = *image_width;
				data->width = *image_height;
				*image_height = data->height;
				*image_width = data->width;
				*max_x = *image_width;
				*max_y = *image_height;
				*min_x = 0;
				*min_y = 0;
				*image = (unsigned char **)malloc(*image_height * sizeof(unsigned char *));
				for (int i = 0; i < *image_height; i++) {
					(*image)[i] = (unsigned char *)malloc(*image_width * sizeof(unsigned char));
				}
		
				if (angle == -90 || angle == 270) {
					for (int i = *min_y; i < *max_y; i++) {
						for (int j = *min_x; j < *max_x; j++) {
							(*image)[i][j] = (*aux)[j][*max_y - 1 - i];
						}
					}
				} else if (angle == 90 || angle == -270) {
					for (int i = *min_y; i < *max_y; i++) {
						for (int j = *min_x; j < *max_x; j++) {
							(*image)[i][j] = (*aux)[*max_x - 1 - j][i];
						}
					}
				}
				free_matrix(*aux, *image_width);
				*aux = (unsigned char **)malloc((*image_height) * sizeof(unsigned char *));
				for (int i = 0; i < *image_height; i++) {
					(*aux)[i] = (unsigned char *)malloc((*image_width) * sizeof(unsigned char));
				}
				for (int i = 0; i < *image_height; i++) {
					for (int j = 0; j < *image_width; j++) {
						(*aux)[i][j] = (*image)[i][j];
					}
				}
			} else {
				if (angle == 180 || angle == -180) {
					for (int i = *min_y; i < *max_y; i++) {
						for (int j = *min_x; j < *max_x; j++) {
							(*image)[i][j] = (*aux)[*max_y - 1 - i][*max_x - 1 - j];
						}
					}
				}
				for (int i = 0; i < *image_height; i++) {
					for (int j = 0; j < *image_width; j++) {
						(*aux)[i][j] = (*image)[i][j];
					}
				}
			}
		}
	} else {
		if (strncmp("P3", data->format, 2) == 0 || strncmp("P6", data->format, 2) == 0) {
			*max_x /= 3;
			if (angle == -90 || angle == 270) {
				for (int i = *min_y; i < *max_y; i++) {
					for (int j = *min_x; j < *max_x; j++) {
						(*image)[i][3 * j] = (*aux)[j][3 * (*min_y + *max_y - 1 - i)];
						(*image)[i][3 * j + 1] = (*aux)[j][3 * (*min_y + *max_y - 1 - i) + 1];
						(*image)[i][3 * j + 2] = (*aux)[j][3 * (*min_y + *max_y - 1 - i) + 2];
					}
				}
			} else if (angle == 90 || angle == -270) {
				for (int i = *min_y; i < *max_y; i++) {
					for (int j = *min_x; j < *max_x; j++) {
						(*image)[i][3 * j] = (*aux)[*min_x + *max_x - 1 - j][3 * i];
						(*image)[i][3 * j + 1] = (*aux)[*min_x + *max_x - 1 - j][3 * i + 1];
						(*image)[i][3 * j + 2] = (*aux)[*min_x + *max_x - 1 - j][3 * i + 2];
					}
				}
			} else if (angle == 180 || angle == -180) {
				for (int i = *min_y; i < *max_y; i++) {
					for (int j = *min_x; j < *max_x; j++) {
						(*image)[i][3 * j] = (*aux)[*min_y + *max_y - 1 - i][3 * (*min_x + *max_x - 1 - j)];
						(*image)[i][3 * j + 1] = (*aux)[*min_y + *max_y - 1 - i][3 * (*min_x + *max_x - 1 - j) + 1];
						(*image)[i][3 * j + 2] = (*aux)[*min_y + *max_y - 1 - i][3 * (*min_x + *max_x - 1 - j) + 2];
					}
				}
			}
			for (int i = 0; i < *image_height; i++) {
				for (int j = 0; j < *image_width; j++) {
					(*aux)[i][j] = (*image)[i][j];
				}
			}
		} else {
			if (angle == -90 || angle == 270) {
				for (int i = *min_y; i < *max_y; i++) {
					for (int j = *min_x; j < *max_x; j++) {
						(*image)[i][j] = (*aux)[j][*min_x + *max_y - 1 - i];
					}
				}
			} else if (angle == 90 || angle == -270) {
				for (int i = *min_y; i < *max_y; i++) {
					for (int j = *min_x; j < *max_x; j++) {
						(*image)[i][j] = (*aux)[*min_y + *max_y - 1 - j][i];
					}
				}
			} else if (angle == 180 || angle == -180) {
				for (int i = *min_y; i < *max_y; i++) {
					for (int j = *min_x; j < *max_x; j++) {
						(*image)[i][j] = (*aux)[*min_y + *max_y - 1 - i][*min_x + *max_x - 1 - j];
					}
				}
			}
			for (int i = 0; i < *image_height; i++) {
				for (int j = 0; j < *image_width; j++) {
					(*aux)[i][j] = (*image)[i][j];
				}
			}
		}
	}
	printf("Rotated %d\n", angle);
}

void copy_matrix(int **image, int **aux, int height, int width)
{
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			aux[i][j] = image[i][j];
		}
	}
}

void image_info(FILE *org_image, img *data)
{
	// char c;
	fseek(org_image, 0, SEEK_SET);
	fscanf(org_image, "%s", data->format);
	fscanf(org_image, "%d", &data->width);
	fscanf(org_image, "%d", &data->height);
	fscanf(org_image, "%d", &data->max_value);
	// fscanf(org_image, "%c", &c);
}

void print_matrix(unsigned char **a, int x, int y)
{
	for (int i = 0; i < x; i++) {
		for (int j = 0; j < y; j++) {
			printf("%hhu ", a[i][j]);
		} 
		printf("\n");
	}
}

int count_digits(int x)
{
	int cnt = 0;
	if (x == 0)
		return 1;
	while (x) {
		cnt++;
		x /= 10;
	}
	return cnt;
}

void free_matrix(unsigned char **a, int height)
{
	for (int i = 0; i < height; i++) {
		free(a[i]);
	}
	free(a);
}

void allocate_matrix(unsigned char **a, int height, int width)
{
	a = (unsigned char **)malloc(height * sizeof(int *));
	for (int i = 0; i < height; i++) {
		a[i] = (unsigned char *)malloc(width * sizeof(int));
	}
}
