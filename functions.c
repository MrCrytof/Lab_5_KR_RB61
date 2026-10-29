
#include "functions.h"
#include <string.h>

// Зчитує вхідні дані з текстового файлу та перевіряє їхню коректність
int readInputData(const char *filename, LabData *data) {
    // Відкриваємо вхідний файл для читання
    FILE *fp_src = fopen(filename, "r");

    if (fp_src == NULL) {
        printf("Error: Cannot open input file %s!\n", filename);
        return 0;
    }

    // Зчитуємо початкове та кінцеве значення аргументу
    if (fscanf(fp_src, "%lf", &data->x_start) != 1 ||
        fscanf(fp_src, "%lf", &data->x_end) != 1 ||
        fscanf(fp_src, "%d", &data->num_points) != 1 ||
        fscanf(fp_src, "%lf", &data->step) != 1) {

        printf("Error: Invalid numerical data in input file!\n");
        fclose(fp_src);
        return 0;
    }

    // Перевіряємо, чи є числові параметри скінченними
    if (!isfinite(data->x_start) ||
        !isfinite(data->x_end) ||
        !isfinite(data->step)) {

        printf("Error: Input values must be finite numbers!\n");
        fclose(fp_src);
        return 0;
    }

    // Перевіряємо коректність діапазону, кількості точок і кроку
    if (data->num_points <= 0 ||
        data->step <= 0.0 ||
        data->x_end < data->x_start) {

        printf("Error: Invalid input parameters!\n");
        fclose(fp_src);
        return 0;
    }

    // Зчитуємо шифр групи, не допускаючи переповнення масиву
    if (fscanf(fp_src, "%49s", data->group) != 1) {
        printf("Error: Cannot read the study group!\n");
        fclose(fp_src);
        return 0;
    }

    // Пропускаємо залишок рядка після шифру групи
    int ch;

    while ((ch = fgetc(fp_src)) != '\n' && ch != EOF) {
        // Пропускаємо символи до кінця поточного рядка
    }

    // Зчитуємо прізвище та ім'я, включно з пробілами
    if (fgets(data->name, sizeof(data->name), fp_src) == NULL) {
        printf("Error: Cannot read the student name!\n");
        fclose(fp_src);
        return 0;
    }

    // Видаляємо символи переведення рядка в кінці імені
    data->name[strcspn(data->name, "\r\n")] = '\0';

    // Перевіряємо, чи не залишилися зайві символи імені поза буфером
    if (strchr(data->name, '\0') == NULL || data->name[0] == '\0') {
        printf("Error: Invalid student name!\n");
        fclose(fp_src);
        return 0;
    }

    // Перевіряємо, чи не виходить остання точка за межі діапазону
    long double last_x =
        (long double)data->x_start +
        (long double)(data->num_points - 1) * data->step;

    if (!isfinite(last_x) ||
        last_x > (long double)data->x_end +
                 1e-12L * fmaxl(1.0L, fabsl((long double)data->x_end))) {

        printf("Error: The number of points and step exceed the range!\n");
        fclose(fp_src);
        return 0;
    }

    // Закриваємо вхідний файл після успішного читання
    fclose(fp_src);

    return 1;
}


// Обчислює функцію та записує результати у текстовий і бінарний файли
int generateAndSaveFiles(
    const LabData *data,
    const char *txtFilename,
    const char *binFilename
) {
    // Відкриваємо текстовий файл для запису
    FILE *fp_txt = fopen(txtFilename, "w");

    if (fp_txt == NULL) {
        printf("Error: Cannot create text file %s!\n", txtFilename);
        return 0;
    }

    // Відкриваємо бінарний файл для запису
    FILE *fp_bin = fopen(binFilename, "wb");

    if (fp_bin == NULL) {
        printf("Error: Cannot create binary file %s!\n", binFilename);
        fclose(fp_txt);
        return 0;
    }

    // Записуємо кількість точок на початку бінарного файлу
    if (fwrite(&data->num_points, sizeof(int), 1, fp_bin) != 1) {
        printf("Error: Cannot write the number of points!\n");
        fclose(fp_txt);
        fclose(fp_bin);
        return 0;
    }

    // Записуємо заголовок текстового файлу
    if (fprintf(fp_txt,
                "Laboratory Work 5 - Variant 7\n"
                "Function: f(x) = -0.5*(x-64)^3 - 3*x^2 + 10\n\n"
                "No.\tArgument (x)\tFunction Value f(x)\n") < 0) {

        printf("Error: Cannot write the text file header!\n");
        fclose(fp_txt);
        fclose(fp_bin);
        return 0;
    }

    // Послідовно обчислюємо значення функції в заданих точках
    for (int i = 0; i < data->num_points; i++) {
        // Обчислюємо аргумент через номер точки, щоб зменшити
        // накопичення похибки від багаторазового додавання кроку
        double x = data->x_start + (double)i * data->step;

        // Обчислюємо значення функції відповідно до варіанта 7
        double fx = -0.5 * pow(x - 64.0, 3)
                    - 3.0 * x * x + 10.0;

        // Перевіряємо результат обчислення
        if (!isfinite(x) || !isfinite(fx)) {
            printf("Error: Non-finite calculation result!\n");
            fclose(fp_txt);
            fclose(fp_bin);
            return 0;
        }

        // Записуємо рядок таблиці у текстовий файл
        if (fprintf(fp_txt, "%d\t%.4f\t%.4f\n",
                    i + 1, x, fx) < 0) {

            printf("Error: Cannot write table data to text file!\n");
            fclose(fp_txt);
            fclose(fp_bin);
            return 0;
        }

        // Записуємо аргумент і значення функції у бінарний файл
        if (fwrite(&x, sizeof(double), 1, fp_bin) != 1 ||
            fwrite(&fx, sizeof(double), 1, fp_bin) != 1) {

            printf("Error: Cannot write data to binary file!\n");
            fclose(fp_txt);
            fclose(fp_bin);
            return 0;
        }
    }

    // Записуємо інформацію про студента після таблиці
    if (fprintf(fp_txt, "\nStudy Group: %s\nStudent: %s\n",
                data->group, data->name) < 0) {

        printf("Error: Cannot write student information!\n");
        fclose(fp_txt);
        fclose(fp_bin);
        return 0;
    }

    // Перевіряємо успішність закриття обох файлів
    int txt_error = fclose(fp_txt);
    int bin_error = fclose(fp_bin);

    if (txt_error != 0 || bin_error != 0) {
        printf("Error: Failed to finish writing output files!\n");
        return 0;
    }

    return 1;
}


// Зчитує дані з бінарного файлу в динамічний масив і виводить таблицю
int readAndPrintBinaryFile(const char *binFilename) {
    // Відкриваємо бінарний файл для читання
    FILE *fp_bin = fopen(binFilename, "rb");

    if (fp_bin == NULL) {
        printf("Error: Cannot open binary file %s!\n", binFilename);
        return 0;
    }

    // Зчитуємо кількість точок із початку бінарного файлу
    int n_read = 0;

    if (fread(&n_read, sizeof(int), 1, fp_bin) != 1) {
        printf("Error: Cannot read the number of points!\n");
        fclose(fp_bin);
        return 0;
    }

    // Перевіряємо кількість точок перед виділенням пам'яті
    if (n_read <= 0 ||
        (size_t)n_read > SIZE_MAX / (2 * sizeof(double))) {

        printf("Error: Invalid number of points in binary file!\n");
        fclose(fp_bin);
        return 0;
    }

    // Виділяємо пам'ять для 2*N значень типу double
    double *dynamic_array =
        malloc((size_t)2 * (size_t)n_read * sizeof(double));

    if (dynamic_array == NULL) {
        printf("Error: Dynamic memory allocation failed!\n");
        fclose(fp_bin);
        return 0;
    }

    // Зчитуємо пари значень: аргумент x та функцію f(x)
    for (int i = 0; i < n_read; i++) {
        if (fread(&dynamic_array[2 * i],
                  sizeof(double), 1, fp_bin) != 1 ||
            fread(&dynamic_array[2 * i + 1],
                  sizeof(double), 1, fp_bin) != 1) {

            printf("Error: Failed to read binary data!\n");
            free(dynamic_array);
            fclose(fp_bin);
            return 0;
        }
    }

    // Закриваємо бінарний файл після читання
    fclose(fp_bin);

    // Виводимо зчитані дані у вигляді таблиці
    printf("\n=== TABLE OF DATA READ FROM BINARY FILE ===\n");
    printf("+-------+--------------------+----------------------+\n");
    printf("| Point |    Argument (x)    |  Function Value f(x) |\n");
    printf("+-------+--------------------+----------------------+\n");

    for (int i = 0; i < n_read; i++) {
        // Отримуємо аргумент і значення функції з динамічного масиву
        double curr_x = dynamic_array[2 * i];
        double curr_fx = dynamic_array[2 * i + 1];

        // Виводимо поточний рядок таблиці
        printf("| %-5d | %18.4f | %20.4f |\n",
               i + 1, curr_x, curr_fx);
    }

    printf("+-------+--------------------+----------------------+\n");

    // Звільняємо пам'ять після завершення роботи з масивом
    free(dynamic_array);

    return 1;
}

void test (int quantity)
{
    for (int i = 0; i < quantity; i++)
    {
        printf("Hello world!\n");
    }
}
