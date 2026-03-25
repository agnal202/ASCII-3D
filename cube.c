#include "tc.h"
#include <math.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

int W, H, colornum = 17;

int Wconst = 3;
// tinker with Wconst according to your font (changes with fontsize) | options
// options [1-3]

void handle_winz(int signum) {
  get_terminal_size(&H, &W);
  W /= Wconst;
}

void drawLine(int x1, int y1, int x2, int y2) {
  int dx = abs(x2 - x1);
  int dy = abs(y2 - y1);
  int sx = (x1 < x2) ? 1 : -1;
  int sy = (y1 < y2) ? 1 : -1;
  int err = dx - dy;

  while (1) {
    if (x1 > 0 && y1 > 0 && x1 < W && y1 < H) {
      printf("\e[%d;%dH\e[48;5;%dm   \e[m", y1, x1 * Wconst, colornum);
    }
    if (x1 == x2 && y1 == y2)
      break;

    int e2 = 2 * err;
    if (e2 > -dy) {
      err -= dy;
      x1 += sx;
    }
    if (e2 < dx) {
      err += dx;
      y1 += sy;
    }
  }
}
typedef struct {
  int sx, sy;      // starting point
  int cx, cy;      // current point
  int started;     // flag for begin()
  int initialized; // flag for first draw
} LineDrawer;

LineDrawer drawer = {0, 0, 0, 0, 0, 0};

void beginShape() {
  drawer.started = 1;
  drawer.initialized = 0; // not yet initialized
}

void draw(int x, int y) {
  if (!drawer.started) {
    printf("Error: You must call beginShape() first.\n");
    return;
  }

  if (!drawer.initialized) {
    // First draw initializes start and current point
    drawer.sx = x;
    drawer.sy = y;
    drawer.cx = x;
    drawer.cy = y;
    drawer.initialized = 1;
    //  printf("Start point set to (%d, %d)\n", x, y);
  } else {
    drawLine(drawer.cx, drawer.cy, x, y);
    drawer.cx = x;
    drawer.cy = y;
  }
}

void closeShape() {
  if (!drawer.started) {
    printf("Error: You must call beginShape() first.\n");
    return;
  }
  if (!drawer.initialized) {
    printf("Error: No points drawn yet.\n");
    return;
  }

  drawLine(drawer.cx, drawer.cy, drawer.sx, drawer.sy);
  drawer.started = 0;
  drawer.initialized = 0;
}

typedef struct {
  float x, y, z;
} Vertex;

typedef struct {
  Vertex v[4]; // 4 vertices per face
} Face;

Face cube[] = {
    // Top face (y = 0.5)
    {{{-0.5f, 0.5f, -0.5f},
      {0.5f, 0.5f, -0.5f},
      {0.5f, 0.5f, 0.5f},
      {-0.5f, 0.5f, 0.5f}}},

    // Bottom face (y = -0.5)
    {{{-0.5f, -0.5f, -0.5f},
      {0.5f, -0.5f, -0.5f},
      {0.5f, -0.5f, 0.5f},
      {-0.5f, -0.5f, 0.5f}}},

    // Front face (z = 0.5)
    {{{-0.5f, 0.5f, 0.5f},
      {0.5f, 0.5f, 0.5f},
      {0.5f, -0.5f, 0.5f},
      {-0.5f, -0.5f, 0.5f}}},

    // Back face (z = -0.5)
    {{{-0.5f, 0.5f, -0.5f},
      {0.5f, 0.5f, -0.5f},
      {0.5f, -0.5f, -0.5f},
      {-0.5f, -0.5f, -0.5f}}}};

double convert(float xy, float z, double angle) {
  return (xy / ((z - 6) * tan(angle / 2)));
}

int main() {
  double angle;
  double x, y;
  int temp;
  char ch;

  struct sigaction sa;

  sa.sa_handler = handle_winz;
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = 0;

  sigaction(SIGWINCH, &sa, NULL);

  get_terminal_size(&H, &W);
  W = W / Wconst;

  printf("  angle : ");
  scanf("%lf", &angle);

  angle = angle * (M_PI / 180);

  double rotA;
  float rotx, roty, rotz;

  for (int rot = 0; 1; rot++, rot %= 360) {
    if (rot % 30 == 0) {
      if (colornum > 232)
        colornum = 16;
      else
        colornum++;
    }

    rotA = rot * (M_PI / 180);

    for (int face = 0; face < 4; face++) {
      beginShape();
      for (int vert = 0; vert < 4; vert++) {
        // Multiplication using rotation matrices
        rotx =
            cube[face].v[vert].x * cos(rotA) + cube[face].v[vert].z * sin(rotA);
        roty = cube[face].v[vert].x * (sin(rotA) * sin(rotA)) +
               cube[face].v[vert].y * cos(rotA) -
               cube[face].v[vert].z * sin(rotA) * cos(rotA);
        rotz = -cube[face].v[vert].x * sin(rotA) * cos(rotA) +
               cube[face].v[vert].y * sin(rotA) +
               cube[face].v[vert].z * (cos(rotA) * cos(rotA));

        x = convert(rotx * H / W, rotz, angle);
        y = -convert(roty, rotz, angle);

        draw(x * W + (float)W / 2, y * H + (float)H / 2);
      }
      closeShape();
    }
    fflush(stdout);
    usleep(10000);
    printf("\e[2J");
  }
}
