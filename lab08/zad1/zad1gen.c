#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define TEXT_LENGTH 1020
#define RECORDS_COUNT 100

int nextId = 26;

struct record {
    int id;
    char text[TEXT_LENGTH];
};

int main(int argc, char const *argv[]) {

    char const *fileName = "genTest2.txt";

    if(argc > 1) {
        fileName = argv[1];
    }

    FILE *file;

    if ((file = fopen(fileName, "w")) == NULL) {
        perror("Error while opening the file.\n");
        exit(EXIT_FAILURE);
    }

    int i;
    for (i = 0; i < RECORDS_COUNT; i++) {
        struct record rec;
        rec.id = nextId++;
        sprintf(rec.text,
                "Ut pellentesque ligula neque, ut tincidunt orci varius eu. Nunc iaculis, enim nec tempus aliquet, "
                        "arcu diam porttitor sem, ac scelerisque libero erat nec nulla. Curabitur lacinia aliquet pulvinar. "
                        "Duis placerat lacus sit amet magna suscipit, sed faucibus eros mattis. Fusce tempor sagittis "
                        "sapien faucibus fermentum. Aliquam semper sapien vitae blandit gravida. Vivamus sit amet malesuada orci. "
                        "Vivamus ultricies felis vel sem vehicula, nec accumsan arcu eleifend. Suspendisse vulputate "
                        "lacus sit amet dui convallis, sed ullamcorper eros ultricies. Etiam nec lorem non purus semper "
                        "fringilla. Spellentesque id pulvinar nibh. Nulla id diam id elit posuere aliquam. "
                        "Pellentesque eget lacus eget nisl fringilla ultricies eget vitae sapien. Nulla cursus et "
                        "neque vitae hendrerit. Donec condimentum nisi in diam porta, at pretium sapien gravida. "
                        "Sed in dignissim turpis. Donec tellus nisl. Phasellus blandit nisl eget egestas molestie. "
                        "Vestibulum at augue felis. Integer nulla velit, fermentum efficitur pulvinar massa nunc.");

//        printf("%d\n", strlen(rec.text));

        fwrite(&rec, sizeof(struct record), 1, file);
    }

    fclose(file);
    return 0;
}