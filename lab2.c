#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <string.h>

double **matrix, **temp;
int N, M, K, W, max_threads;
sem_t semaphore;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
int current_row = 0;
int threads_completed = 0;

void print_matrix(double **mat, const char *title) {
    printf("%s:\n", title);
    for (int i = 0; i < (N > 5 ? 5 : N); i++) {
        for (int j = 0; j < (M > 5 ? 5 : M); j++) {
            printf("%8.2f", mat[i][j]);
        }
        printf("\n");
    }
    if (N > 5 || M > 5) {
        printf("... (показаны первые 5x5 элементов)\n");
    }
    printf("\n");
}

void apply_filter(int row) {
    int half = W / 2;
    for (int j = 0; j < M; j++) {
        double sum = 0.0;
        int count = 0;
        
        for (int wi = -half; wi <= half; wi++) {
            for (int wj = -half; wj <= half; wj++) {
                int ni = row + wi;
                int nj = j + wj;
                
                if (ni >= 0 && ni < N && nj >= 0 && nj < M) {
                    sum += matrix[ni][nj];
                    count++;
                }
            }
        }
        temp[row][j] = sum / count;
    }
}

void* worker() {
    for (int k = 0; k < K; k++) {
        sem_wait(&semaphore);
        
        // Обработка строк
        while (1) {
            pthread_mutex_lock(&mutex);
            int row = current_row++;
            pthread_mutex_unlock(&mutex);
            
            if (row >= N) break;
            apply_filter(row);
        }
        
        // Синхронизация между итерациями
        pthread_mutex_lock(&mutex);
        threads_completed++;
        
        if (threads_completed == max_threads) {
            // Последний поток выполняет обмен матриц
            printf("  Итерация %d завершена всеми потоками\n", k + 1);
            
            double **swap = matrix;
            matrix = temp;
            temp = swap;
            
            current_row = 0;
            threads_completed = 0;
            pthread_cond_broadcast(&cond);
        }  else {
            // Ожидание завершения другими потоками
            pthread_cond_wait(&cond, &mutex);
        }
        pthread_mutex_unlock(&mutex);
        
        sem_post(&semaphore);
    }
    return NULL;
}

int main(int argc, char *argv[]) {
    if (argc != 5) {
        printf("Необходимо ввести: ./lab2 <размер_окна> <итерации> <потоки> <размер_матрицы>\n");
        return 1;
    }
    
    W = atoi(argv[1]);
    K = atoi(argv[2]);
    max_threads = atoi(argv[3]);
    N = M = atoi(argv[4]);
    
    if (W % 2 == 0) {
        printf("Размер окна должен быть нечетным!\n");
        return 1;
    }

    printf("\nПараметры обработки:\n");
    printf("  Размер окна: %d\n", W);
    printf("  Количество итераций: %d\n", K);
    printf("  Максимальное количество потоков: %d\n", max_threads);
    printf("  Размер матрицы: %dx%d\n\n", N, M);

    matrix = malloc(N * sizeof(double*));
    temp = malloc(N * sizeof(double*));
    
    for (int i = 0; i < N; i++) {
        matrix[i] = malloc(M * sizeof(double));
        temp[i] = malloc(M * sizeof(double));
        
        // Инициализация матрицы случайными значениями
        for (int j = 0; j < M; j++) {
            matrix[i][j] = rand() % 100;
            temp[i][j] = 0.0;
        }
    }

    print_matrix(matrix, "Исходная матрица");

    sem_init(&semaphore, 0, max_threads);

    printf("Запуск обработки...\n");
    
    // Создание потоков
    pthread_t threads[max_threads];
    for (int i = 0; i < max_threads; i++) {
        pthread_create(&threads[i], NULL, worker, NULL);
    }

    // Ожидание завершения потоков
    for (int i = 0; i < max_threads; i++) {
        pthread_join(threads[i], NULL);
    }

    printf("Обработка завершена!\n\n");

    print_matrix(matrix, "Результат обработки");

    printf("\n");

    for (int i = 0; i < N; i++) {
        free(matrix[i]);
        free(temp[i]);
    }
    free(matrix);
    free(temp);
    sem_destroy(&semaphore);
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond);

    return 0;
}