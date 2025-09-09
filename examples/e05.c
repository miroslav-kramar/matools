#define MA_PARSE_IMPLEMENTATION
#include "../ma_parse.h"
#include <stdio.h>

typedef struct {
	char fname[32];
	char lname[32];
	char city[32];
	double height;
	int age;
} Person;

void person_print(Person * p) {
	printf("Name: %s %s\n", p->fname, p->lname);
	printf("Age: %d, Height: %.2lf\n", p->age, p->height);
	printf("Lives in: %s", p->city);
}

int main() {
	FILE * fp = fopen("people.txt", "r");
	if (fp == NULL) {
		printf("Failed to open file!\n");
		return 1;
	}

	Person people[10];
	size_t counter = 0;

	ma_scanner_t sc = ma_scanner_create(fp,",");
	while (1) {
		if (counter >= sizeof(people)/sizeof(*people)) {
			break;
		}

		ma_scanner_get_str(&sc, people[counter].fname, sizeof(people->fname));
		ma_scanner_get_str(&sc, people[counter].lname, sizeof(people->lname));
		people[counter].age    = ma_scanner_get_i32(&sc);
		people[counter].height = ma_scanner_get_dbl(&sc);
		ma_scanner_get_str(&sc, people[counter].city, sizeof(people->city));
		
		if (ma_scanner_get_status(&sc) == MA_STATUS_STREAM_EOF) {
			break;
		}
		if (ma_scanner_get_status(&sc) != MA_STATUS_OK) {
			printf("ERROR: %s\n", ma_status_get_str(ma_scanner_get_status(&sc)));
			return 1;
		}
		else {
			counter++;
		}
	}
	fclose(fp);
	ma_scanner_clear_status(&sc);

	for (size_t i = 0; i < counter; i++) {
		person_print(people+i);
		printf("\n\n");		
	}

	return 0;
}
