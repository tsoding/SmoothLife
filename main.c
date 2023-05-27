#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#define WIDTH 100
#define HEIGHT 100

char level[] = " .-=coaA@#";
#define level_count (sizeof(level)/sizeof(level[0]) - 1)

float grid[HEIGHT][WIDTH] = {0};
float grid_diff[HEIGHT][WIDTH] = {0};
float ra = 11;
float alpha = 0.028;
// float alpha = 0.147;
float b1 = 0.278;
float b2 = 0.365;
float d1 = 0.267;
float d2 = 0.445;
float dt = 0.05f;

float rand_float(void)
{
    return (float)rand()/(float)RAND_MAX;
}

void random_grid(void)
{
    size_t w = WIDTH/3;
    size_t h = HEIGHT/3;
    for (size_t dy = 0; dy < h; ++dy) {
        for (size_t dx = 0; dx < w; ++dx) {
            size_t x = dx + WIDTH/2 - w/2;
            size_t y = dy + HEIGHT/2 - h/2;
            grid[y][x] = rand_float();
        }
    }
}

void display_grid(float grid[HEIGHT][WIDTH])
{
    for (size_t y = 0; y < HEIGHT; ++y) {
        for (size_t x = 0; x < WIDTH; ++x) {
            char c = level[(int)(grid[y][x]*(level_count - 1))];
            fputc(c, stdout);
            fputc(c, stdout);
        }
        fputc('\n', stdout);
    }
}

int emod(int a, int b)
{
    return (a%b + b)%b;
}

float sigma1(float x, float a)
{
    return 1.0f/(1.0f + expf(-(x - a)*4/alpha));
}

float sigma2(float x, float a, float b)
{
    return sigma1(x, a)*(1 - sigma1(x, b));
}

float sigmam(float x, float y, float m)
{
    return x*(1 - sigma1(m, 0.5f)) + y*sigma1(m, 0.5f);
}

float s(float n, float m)
{
    return sigma2(n, sigmam(b1, d1, m), sigmam(b2, d2, m));
}

void compute_grid_diff(void)
{
    for (int cy = 0; cy < HEIGHT; ++cy) {
        for (int cx = 0; cx < WIDTH; ++cx) {
            float m = 0, M = 0;
            float n = 0, N = 0;
            float ri = ra/3;

            for (int dy = -(ra - 1); dy <= (ra - 1); ++dy) {
                for (int dx = -(ra - 1); dx <= (ra - 1); ++dx) {
                    int x = emod(cx + dx, WIDTH);
                    int y = emod(cy + dy, HEIGHT);
                    if (dx*dx + dy*dy <= ri*ri) {
                        m += grid[y][x];
                        M += 1;
                    } else if (dx*dx + dy*dy <= ra*ra) {
                        n += grid[y][x];
                        N += 1;
                    }
                }
            }
            m /= M;
            n /= N;
            float q = s(n, m);
            grid_diff[cy][cx] = 2*q - 1;
        }
    }
}

void clamp(float *x, float l, float h)
{
    if (*x < l) *x = l;
    if (*x > h) *x = h;
}

int main(void)
{
    srand(time(0));
    random_grid();

    display_grid(grid);
    for (;;) {
        compute_grid_diff();
        for (size_t y = 0; y < HEIGHT; ++y) {
            for (size_t x = 0; x < WIDTH; ++x) {
                grid[y][x] += dt*grid_diff[y][x];
                clamp(&grid[y][x], 0, 1);
            }
        }
        display_grid(grid);
    }

    return 0;
}
