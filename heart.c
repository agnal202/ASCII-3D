#include "tc.h"
#include <math.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

int W, H, Wconst = 3;
int CAMERA_D = 120;

void handle_winz(int signum) {
  get_terminal_size(&H, &W);
  W /= Wconst;
  printf("\e[2J");
}
double convert(float xy, float z, double angle) {
  return (xy / ((z - CAMERA_D) * tan(angle / 2)));
}

void heart2D(double T, double angle) {
  int x, y;

  float hx, hy, hz = 0, tx, ty, tz;

  float cosT = cos(T), sinT = sin(T);

  for (int t = 0; t < 360; t++) {
    hx = (15 * pow(sin(t), 3));
    hy = (13 * cos(t) - 5 * cos(2 * t) - 2 * cos(3 * t) - cos(4 * t));
    hz = 0;

    tx = hx;
    ty = hy;
    tz = hz;

    hx = tx * cosT + tz * sinT;
    hz = -tx * sinT + tz * cosT;

    x = (convert(hx * H / W, hz, angle) * W + (float)W / 2);
    y = (convert(hy, hz, angle) * H + (float)H / 2);

    if (x > 0 && y > 0 && x < W && y < H)
      printf("\e[%d;%dH\e[41m  \e[m", y, x * Wconst);
  }
}
void heart3D(double T, double angle) {
  int x, y;
  float hx, hy, hz = 0, tx;
  float bface;
  double u, v;

  for (int U = 0; U <= 360; U += 5) {
    u = U * (M_PI / 180);

    for (int V = 0; V <= 180; V += 10) {
      v = V * (M_PI / 180);

      hx = sin(v) * (15 * sin(u) - 4 * sin(3 * u));
      hy =
          sin(v) * (15 * cos(u) - 5 * cos(2 * u) - 2 * cos(3 * u) - cos(2 * u));
      hz = 8 * cos(v);

      tx = hx;

      hx = tx * cos(T) + hz * sin(T);
      hz = -tx * sin(T) + hz * cos(T);

      x = (convert(hx * H / W, hz, angle) * W + (float)W / 2);
      y = (convert(hy, hz, angle) * H + (float)H / 2);

      if (x > 0 && y > 0 && x < W && y < H)
        printf("\e[%d;%dH\e[1;91m* \e[m", y, x * Wconst);
    }
  }
}
int main() {
  double angle = 50;

  struct sigaction sa;

  sa.sa_handler = handle_winz;
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = 0;

  sigaction(SIGWINCH, &sa, NULL);

  get_terminal_size(&H, &W);
  W = W / Wconst;

  // printf("  angle = ");
  //	scanf("%lf", &angle);

  angle = angle * (M_PI / 180);

  double T;

  int is3D = 0;

  printf("for 3D enter ( 1 ) : ");
  is3D = getchar() == 1 ? 1 : 0;

  printf("\e[?25l");

  for (int theta = 0; 1; theta++, theta %= 360) {
    T = theta * (M_PI / 180);

    if (is3D)
      heart3D(T, angle);
    else
      heart2D(T, angle);

    fflush(stdout);
    usleep(50000);
    printf("\e[2J");
  }
  printf("\e[?25h");
}

/*
x=Sin[v](15 Sin[u]-4 Sin[3u])

y=8 Cos[v]

z=Sin[v](15 Cos[u]-5 Cos[2u]-2 Cos[3u]-Cos[2u])
*/
