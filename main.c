#define _CRT_RAND_S
#include <direct.h>
#include <math.h>
#include <stdio.h>
#include <time.h>
#include <windows.h>
#include "dirent.h"

#define W 256
#define A 256

void LineOptimized(int x1, int y1, int x2, int y2, unsigned char** mat);

void binario(int b, char* vet) {
  int i = 0;     // Inizializza indice
  while (i < 7)  // Fino all'untimo elemento
  {
    if ((b % 2) == 0)  // Se il bit meno significativo e' 0
    {
      vet[7 - 1 - i] = '0';  // metto 0 nell'elemento del vettore
    } else {
      vet[7 - 1 - i] = '1';  // se no, metto 1
    }
    b = b / 2;  // Elimino ilbit meno significativo per passare al prossimo
    ++i;        // conto i bit elaborati
  }
}

void setPixel(int x, int y, unsigned char** mat) {
  if (x >= 0 && y >= 0 && x <= (A - 1) && y <= (W - 1)) mat[x][y] = 1;
  return;
}

void Fill_circle(const int x, const int y, const int r, unsigned char** mat) {
  int x1;
  int x2;
  int counter = (y + r);
  int count;

  for (count = (y - r); count <= counter; count++) {
    x1 = (int)(x + sqrt((r * r) - ((count - y) * (count - y))) + 0.5);
    x2 = (int)(x - sqrt((r * r) - ((count - y) * (count - y))) + 0.5);

    LineOptimized(x1, count, x2, count, mat);
  }
}

#define PI 3.1415926535897932384626433832795f

void LineOptimized(int x1, int y1, int x2, int y2, unsigned char** mat) {
  int cx, cy, ix, iy, dx, dy, ddx = x2 - x1, ddy = y2 - y1;

  if (!ddx) {  // caso speciale linea verticale
    if (ddy > 0) {
      cy = y1;
      do
        setPixel(x1, cy++, mat);
      while (cy <= y2);
      return;
    } else {
      cy = y2;
      do
        setPixel(x1, cy++, mat);
      while (cy <= y1);
      return;
    }
  }
  if (!ddy) {  // caso speciale linea orizzontale
    if (ddx > 0) {
      cx = x1;
      do
        setPixel(cx, y1, mat);
      while (++cx <= x2);
      return;
    } else {
      cx = x2;
      do
        setPixel(cx, y1, mat);
      while (++cx <= x1);
      return;
    }
  }
  if (ddy < 0) {
    iy = -1;
    ddy = -ddy;
  }  // scrivo sopra
  else
    iy = 1;
  if (ddx < 0) {
    ix = -1;
    ddx = -ddx;
  }  // scrivo sinistra
  else
    ix = 1;
  dx = dy = ddx * ddy;
  cy = y1, cx = x1;
  if (ddx < ddy) {  // inclinazione minore dei < 45°, linea lunga
    do {
      dx -= ddy;
      do {
        setPixel(cx, cy, mat);
        cy += iy, dy -= ddx;
      } while (dy >= dx);
      cx += ix;
    } while (dx > 0);
  } else {  // inclinazione minore dei >= 45° , linea larga
    do {
      dy -= ddx;
      do {
        setPixel(cx, cy, mat);
        cx += ix, dx -= ddy;
      } while (dx >= dy);
      cy += iy;
    } while (dy > 0);
  }
}

int casual(int max) {
  unsigned int number;
  rand_s(&number);
  return (unsigned int)((double)number / ((double)UINT_MAX + 1) * max +
                        0 /*min*/);
}

int casual2(int max, int min) {
  unsigned int number;
  rand_s(&number);
  return (int)((double)number / ((double)UINT_MAX + 1) * (max - min)) + min;
}

int pgmwrite(char* filename, int w, int h, unsigned char** data,
             char* comment_string) {
  FILE* file;
  int maxval;
  int i, j;
  errno_t err;
  if ((err = fopen_s(&file, filename, "w")) != 0) {
    printf("ERRORE: apertura file\n");
    return (-1);
  }
  fprintf(file, "P2\n");
  if (comment_string != NULL) fprintf(file, "#%s\n", comment_string);
  fprintf(file, "%d\n%d\n", w, h);
  maxval = 1;
  fprintf(file, "%d\n", maxval);
  for (i = 0; i < h; i++) {
    for (j = 0; j < w; j++) {
      if (j == (h - 1))
        fprintf(file, "%d\n", data[i][j]);
      else
        fprintf(file, "%d ", data[i][j]);
    }
  }
  printf("Scrivo il file %s.\n", filename);
  fclose(file);
  return (0);
}

float trasforma(int val, int min, int max) {
  return (((float)(val - min)) / ((float)(max - min)));
}

typedef struct CR {
  int x;
  int y;
  int r;
} cerchio;

int get_num_cerchi() {
  char c[100];
  int d = 1;
  while (1) {
    printf("\n**inserisci numero di immagini da creare(min 1 max 10000): \n");
    memset(c, 0, 100);
    scanf_s("%99s", c, 100);
    d = atoi(c);
    fflush(stdin);
    if (d >= 1 || d <= 10000) break;
    printf("\n**scegli un valore tra 1 e 2**");
  }
  printf("\n");
  return d;
}

int main() {
  int i, j, k = 0, num, num1 = 0, x, y, r, ok, cir, H, L, *count, TotCerchi,
            max, cas1, cas2;
  unsigned char*** mat2;
  char **filename, val[100], prova[1000], Buffer[1000];
  struct tm ora;
  cerchio* arr;
  DIR* dir;
  time_t ora_t;
  H = 256;
  L = 256;
  /*procedura per la cattura della directory corrente*/
  _getcwd(Buffer, 1000 * sizeof(char));
  strcat_s(Buffer, 1000, "\\input_files_cerchi\\");
  _chdir(Buffer);  // cambia la posizione della cartella di lavoro
  /*apre lo stream della directory input */
  if ((dir = opendir(Buffer)) == NULL) {
    printf("\n**errore cartella input**\n");
    system("pause");
    exit(1);
  }
  time(&ora_t);
  localtime_s(&ora, &ora_t);
  strftime(prova, 1000, "_%d_%b_%Y_%H_%M_%S&", &ora);
  TotCerchi = get_num_cerchi();
  mat2 = (unsigned char***)malloc(TotCerchi * sizeof(unsigned char**));
  for (k = 0; k < TotCerchi; k++) {
    mat2[k] = (unsigned char**)calloc(H, sizeof(unsigned char*));
    for (i = 0; i < H; i++)
      mat2[k][i] = (unsigned char*)calloc(L, sizeof(unsigned char));
  }
  count = (int*)malloc(TotCerchi * sizeof(int));
  filename = (char**)malloc(TotCerchi * sizeof(char*));
  for (k = 0; k < TotCerchi; k++) {  // un ciclo per ogni immagine
    filename[k] = (char*)calloc(500, sizeof(char));
    memset(val, 0, 100);
    for (i = 0; i < H; i++) memset(mat2[k][i], 0, L * sizeof(unsigned char));
    num1 = 0;
    strcpy_s(filename[k], 500, "immagine_");
    if (k == casual2(TotCerchi, 0))
      num = 25;
    else
      num = casual2(25, 1);  // numero cerchi che al massimo si possono
                             // disegnare
    if (k == 0) arr = (cerchio*)malloc(25 * sizeof(cerchio));
    // if(k>0 && last>num)realloc( arr, last * sizeof(cerchio) );
    memset(arr, 0, 25 * sizeof(cerchio));
    for (i = 0; i < num; i++) {
      cir = 1;
      ok = 0;
      r = casual2(
          45, 10);  // valori tra 10 e 45 il minimo supportato dalla risoluzione
      x = casual2(H - r, r);
      y = casual2(H - r, r);
      if (x >= r && x < (H - r) && y > r && y < (H - r)) {
        if (num1 > 0) {
          for (j = 0; j < num1; j++) {
            if ((abs(x - arr[j].x) < (r + arr[j].r)) &&
                (abs(y - arr[j].y) < (r + arr[j].r))) {
              ok = 1;
              break;
            } else {
              ok = 0;
            }
          }
        }
        if (!ok) {
          Fill_circle(x, y, r, mat2[k]);
          arr[num1].x = x;
          arr[num1].y = y;
          arr[num1].r = r;
          num1++;
        }
      }
    }
    _itoa_s(k, val, 100, 10);
    strcat_s(filename[k], 500, val);
    strcat_s(filename[k], 500, prova);
    strcat_s(filename[k], 500, ".PGM");
    count[k] = num1;
  }
  memset(val, 0, 100);
  //	for (max=count[0],k=1;k<TotCerchi;k++)if
  //(count[k]>max)max=count[k];//ricerco il max
  max = 25;
  for (k = 0; k < TotCerchi; k++) {
    sprintf_s(val, 100, "%f", trasforma(count[k], 0, max));  // eseguo la
                                                             // trasformazione
                                                             // degli output per
                                                             // la func sigmoide
    pgmwrite(filename[k], L, H, mat2[k], val);
  }
  return 0;
}