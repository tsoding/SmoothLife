#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#define WIDTH 128
#define HEIGHT 128

//#define TERM_COLORS

char level[] = " .,'`_^\"-<:;=!|([{oO0&%$@#";
#define level_count (sizeof(level)/sizeof(level[0]) - 1)

float grid[HEIGHT][WIDTH] = {0};
float grid_diff[HEIGHT][WIDTH] = {0};
float ra = 11;
float alpha_n = 0.028;
float alpha_m = 0.147;
float b1 = 0.278;
float b2 = 0.365;
float d1 = 0.267;
float d2 = 0.445;
float dt = 0.01f;

float rand_float(void)
{
	return (float)rand()/(float)RAND_MAX;
}

void random_grid(void)
{
	size_t w = WIDTH/2;
	size_t h = HEIGHT/2;
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
	char buf[WIDTH*HEIGHT*
	#ifdef TERM_COLORS
		20
	#else
		1
	#endif
		+ 1];
	int off = 0;

	off += sprintf(buf+off, "\x1b[H");
	for (size_t y = 0; y < HEIGHT-1; y+=2) {
		for (size_t x = 0; x < WIDTH; x++) {
			float val = 0.5f*(grid[y][x] + grid[y+1][x]);
		#ifdef TERM_COLORS
			unsigned char l = (unsigned char)(val*255.99) % 256;
			off += sprintf(buf+off, "\x1b[48;2;%d;%d;%dm ", l, l, l);
		#else
			char c = level[(int)(val*(level_count - 1))];
			buf[off++] = c;
		#endif
		}
		off += sprintf(buf+off,
	#ifdef TERM_COLORS
		"\x1b[0m"
	#endif
		"\n");
	}
	printf("%.*s", off, buf);
}

int emod(int a, int b)
{
	return (a%b + b)%b;
}

float sigma(float x, float a, float alpha)
{
	return 1.0f/(1.0f + expf(-(x - a)*4/alpha));
}

float sigma_n(float x, float a, float b)
{
	return sigma(x, a, alpha_n)*(1 - sigma(x, b, alpha_n));
}

float sigma_m(float x, float y, float m)
{
	return x*(1 - sigma(m, 0.5f, alpha_m)) + y*sigma(m, 0.5f, alpha_m);
}

float s(float n, float m)
{
	return sigma_n(n, sigma_m(b1, d1, m), sigma_m(b2, d2, m));
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

void apply_grid_diff(void)
{
	for (size_t y = 0; y < HEIGHT; ++y) {
		for (size_t x = 0; x < WIDTH; ++x) {
			grid[y][x] += dt*grid_diff[y][x];
			clamp(&grid[y][x], 0, 1);
		}
	}
}

int main(void)
{
	srand(time(0));
	random_grid();

	display_grid(grid);
	for (;;) {
		compute_grid_diff();
		apply_grid_diff();
		display_grid(grid);
	}

	return 0;
}
