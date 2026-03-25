#include "tc.h"
#include <math.h>
#include <signal.h>
#include <stdio.h>

int W, H;
int Wconst = 3;

void handle_winz(int signum) {
  get_terminal_size(&H, &W);
  W /= Wconst;
  printf("\e[2J");
}
double convert(float xy, float z, double angle) {
  return (xy / ((z - 40) * tan(angle / 2)));
}

void star(double Y, double angle) {
  int x, y;
  float r = 2.6;

  float sx, sy, sz = 0, tx, ty, tz;

  float cosY = cos(Y), sinY = sin(Y);
  float cosP, sinP, cosZ, sinZ;

  for (float Z = 0; Z < 6.29; Z += 1.25) {
    cosZ = cos(Z);
    sinZ = sin(Z);

    for (float P = 0; P < 6.29; P += 0.02) {
      cosP = cos(P);
      sinP = sin(P);

      sx = (8 * cosP * cosZ - r * sinZ) * cosY + 1.5 * pow(sinP, 3) * sinY;
      sy = 8 * cosP * sinZ + r * cosZ;
      sz = -(8 * cosP * cosZ - r * sinZ) * sinY + 1.5 * pow(sinP, 3) * cosY;

      tx = sx;
      ty = sy;

      sx = tx * cosY - ty * sinY;
      sy = (tx * sinY + ty * cosY);

      x = (convert(sx * H / W, sz, angle) * W + (float)W / 2);
      y = (convert(sy, sz, angle) * H + (float)H / 2);

      // if (x < 0)
      //   x += W;
      // else if (x > W)
      //   x -= W;

      if (x > 0 && y > 0 && x < W && y < H)
        printf("\e[%d;%dH\x1b[103m , \e[m", y, x * Wconst);
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

  printf("  angle : ");
  scanf("%lf", &angle);

  angle = angle * (M_PI / 180);

  double T;

  for (int t = 0;; t += 1, t %= 360) {
    T = t * (M_PI / 180);

    star(T, angle);
    fflush(stdout);
    usleep(10000);
    printf("\e[2J");
  }
}

/* Math for pointy oval
sy = 8 * cos(t);
sx = 1.5 * pow(sin(t),3);
sz = 1;
*/
