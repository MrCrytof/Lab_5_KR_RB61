
#include "functions.h"

int main(void) {
    test(4);
    // Оголошуємо структуру для збереження вхідних даних
    LabData data;

    // Задаємо назви вхідного та вихідних файлів
    const char *inputTxt = "source_file.txt";
    const char *outputTxt = "dest_file.txt";
    const char *outputBin = "test_3.bin";

    // 1. Зчитуємо вхідні дані з текстового файлу
    if (!readInputData(inputTxt, &data)) {
        printf("Execution stopped due to input file error.\n");
        return 1;
    }

    printf("Input data successfully read from %s.\n", inputTxt);

    // 2. Обчислюємо функцію та створюємо вихідні файли
    if (!generateAndSaveFiles(&data, outputTxt, outputBin)) {
        printf("Execution stopped due to file generation error.\n");
        return 1;
    }

    printf("Text file [%s] and binary file [%s] successfully created.\n",
           outputTxt, outputBin);

    // 3. Зчитуємо дані з бінарного файлу та виводимо їх на екран
    if (!readAndPrintBinaryFile(outputBin)) {
        printf("Execution stopped due to binary read error.\n");
        return 1;
    }

    printf("\nProgram completed successfully!\n");

    return 0;
}
