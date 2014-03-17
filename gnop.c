#include <SDL2/SDL.h>
#include <stdio.h>

inline static double gettime(void) {
	return (
		(double) SDL_GetPerformanceCounter() /
		(double) SDL_GetPerformanceFrequency()
	);
}

inline static double clamp(double val, double min, double max) {
	return (val < min) ? min : (val > max) ? max : val;
}

inline static double mabs(double val) {
	return (val < 0.0) ? - val : val;
}

inline static int within(double val, double chk, double range) {
	return mabs(val - chk) <= range;
}

struct gnop_number {
	/* 5 rows, 4 columns */
	char data[5][4];
};

const struct gnop_number gnop_numbers[] = {
	/* 0 */
	{
		{
			{ 1, 1, 1, 1 },
			{ 1, 0, 0, 1 },
			{ 1, 0, 0, 1 },
			{ 1, 0, 0, 1 },
			{ 1, 1, 1, 1 }
		}
	},
	/* 1 */
	{
		{
			{ 0, 0, 1, 0 },
			{ 0, 1, 1, 0 },
			{ 0, 0, 1, 0 },
			{ 0, 0, 1, 0 },
			{ 0, 1, 1, 1 }
		}
	},
	/* 2 */
	{
		{
			{ 0, 1, 1, 0 },
			{ 1, 0, 0, 1 },
			{ 0, 0, 1, 0 },
			{ 0, 1, 0, 0 },
			{ 1, 1, 1, 1 }
		}
	},
	/* 3 */
	{
		{
			{ 1, 1, 1, 1 },
			{ 0, 0, 0, 1 },
			{ 0, 1, 1, 1 },
			{ 0, 0, 0, 1 },
			{ 1, 1, 1, 1 }
		}
	},
	/* 4 */
	{
		{
			{ 0, 0, 1, 0 },
			{ 0, 1, 1, 0 },
			{ 1, 1, 1, 1 },
			{ 0, 0, 1, 0 },
			{ 0, 0, 1, 0 }
		}
	},
	/* 5 */
	{
		{
			{ 1, 1, 1, 1 },
			{ 1, 0, 0, 0 },
			{ 1, 1, 1, 0 },
			{ 0, 0, 0, 1 },
			{ 1, 1, 1, 0 }
		}
	},
	/* 6 */
	{
		{
			{ 0, 1, 1, 0 },
			{ 1, 0, 0, 0 },
			{ 1, 1, 1, 0 },
			{ 1, 0, 0, 1 },
			{ 0, 1, 1, 0 }
		}
	},
	/* 7 */
	{
		{
			{ 1, 1, 1, 1 },
			{ 0, 0, 0, 1 },
			{ 0, 0, 1, 0 },
			{ 0, 1, 0, 0 },
			{ 0, 1, 0, 0 }
		}
	},
	/* 8 */
	{
		{
			{ 1, 1, 1, 1 },
			{ 1, 0, 0, 1 },
			{ 1, 1, 1, 1 },
			{ 1, 0, 0, 1 },
			{ 1, 1, 1, 1 }
		}
	},
	/* 9 */
	{
		{
			{ 0, 1, 1, 0 },
			{ 1, 0, 0, 1 },
			{ 0, 1, 1, 1 },
			{ 0, 0, 0, 1 },
			{ 0, 1, 1, 0 }
		}
	},
	/* 10 - L */
	{
		{
			{ 1, 0, 0, 0 },
			{ 1, 0, 0, 0 },
			{ 1, 0, 0, 0 },
			{ 1, 0, 0, 0 },
			{ 1, 1, 1, 1 }
		}
	},
	/* 11 - R */
	{
		{
			{ 1, 1, 1, 0 },
			{ 1, 0, 1, 0 },
			{ 1, 1, 1, 0 },
			{ 1, 0, 1, 1 },
			{ 1, 0, 0, 1 }
		}
	},
	/* 12 - N */
	{
		{
			{ 1, 0, 0, 1 },
			{ 1, 1, 0, 1 },
			{ 1, 0, 1, 1 },
			{ 1, 0, 0, 1 },
			{ 0, 0, 0, 0 }
		}
	},
	/* 13 - || */
	{
		{
			{ 1, 0, 0, 1 },
			{ 1, 0, 0, 1 },
			{ 1, 0, 0, 1 },
			{ 1, 0, 0, 1 },
			{ 1, 0, 0, 1 }
		}
	}
};

struct gnop_paddle {
	/* position relative to viewport */
	double x;
	double y;
	/* dimensions relative to viewport */
	double w;
	double h;
	/* points! */
	int points;
};

struct gnop_ball {
	/* speed */
	double xs;
	double ys;
	/* direction */
	double xd;
	double yd;
	/* position relative to viewport */
	double x;
	double y;
	/* dimensions relative to viewport */
	double w;
	double h;
	/* collision yay */
	const struct gnop_paddle *clast;
};

struct gnop_state {
	SDL_bool alive;
	SDL_bool paused;
	SDL_bool vsync;
	int w;
	int h;
	struct gnop_paddle pleft;
	struct gnop_paddle pright;
	struct gnop_ball ball;
	/* 10: L; 11: R; */
	char lwin;
	double ltick;
	double ctick;
	double speed;
};

struct gnop_cfg {
	SDL_Window *win;
	SDL_Renderer *ctx;
	struct gnop_state state;
};

void drawnum(struct gnop_cfg *cfg, int index, double x, double y, double w) {
	int i, j;
	Uint8 r, g, b, a;
	SDL_Rect box, box_bg;
	const struct gnop_number *num = &(gnop_numbers[index]);
	const int rows = sizeof(num->data) / sizeof(num->data[0]);
	const int cols = sizeof(num->data[0]) / sizeof(num->data[0][0]);
	double h = w * (double) rows / (double) cols;
	h *= (double) cfg->state.w / (double) cfg->state.h;
	box.w = (w * cfg->state.w) / cols;
	box.h = (h * cfg->state.h) / rows;
	SDL_GetRenderDrawColor(cfg->ctx, &r, &g, &b, &a);
	for (i = 0; i < rows; i += 1) {
		box.y = cfg->state.h * y - (0.5 * (box.h * rows)) + (i * box.h);
		for (j = 0; j < cols; j += 1) {
			if (num->data[i][j] == 0) {
				continue;
			}
			box.x = cfg->state.w * x - (0.5 * (box.w * cols)) + (j * box.w);
			SDL_SetRenderDrawColor(cfg->ctx, r / 2, g / 2, b / 2, 255);
			box_bg.w = 1.2 * box.w;
			box_bg.h = 1.2 * box.h;
			box_bg.x = box.x;
			box_bg.y = box.y;
			SDL_RenderFillRect(cfg->ctx, &box_bg);
			SDL_SetRenderDrawColor(cfg->ctx, r, g, b, a);
			SDL_RenderFillRect(cfg->ctx, &box);
		}
	}
}

void collide(const struct gnop_paddle *paddle, struct gnop_ball *ball) {
	if (
		ball->clast != paddle &&
		within(paddle->x, ball->x, 0.5 * (paddle->w + ball->w)) &&
		within(paddle->y, ball->y, 0.5 * (paddle->h + ball->h))
	) {
		ball->xd = - ball->xd;
		ball->clast = paddle;
	}
}

void renderinit(struct gnop_cfg *cfg) {
	if (cfg->win == NULL) {
		cfg->ctx = NULL;
		cfg->win = SDL_CreateWindow(
			__FILE__,
			SDL_WINDOWPOS_CENTERED,
			SDL_WINDOWPOS_CENTERED,
			cfg->state.w,
			cfg->state.h,
			SDL_WINDOW_RESIZABLE
		);
	}
	if (cfg->ctx == NULL) {
		int flags = SDL_RENDERER_ACCELERATED;
		if (cfg->state.vsync) {
			flags |= SDL_RENDERER_PRESENTVSYNC;
		}
		printf(
			"creating a renderer with vsync %sabled\n",
			(cfg->state.vsync) ? "en" : "dis"
		);
		cfg->ctx = SDL_CreateRenderer(
			cfg->win,
			-1,
			flags
		);
	}
	SDL_assert_release(cfg->win != NULL);
	SDL_assert_release(cfg->ctx != NULL);
}

void gameinit(struct gnop_cfg *cfg) {
	cfg->state.alive = SDL_TRUE;
	cfg->state.paused = SDL_FALSE;
	cfg->state.vsync = SDL_TRUE;
	cfg->state.speed = 1.0;
	cfg->state.lwin = 12;
	cfg->state.ltick = gettime();
	cfg->state.ball.x = 0.5;
	cfg->state.ball.y = 0.5;
	cfg->state.ball.xs = 0.0;
	cfg->state.ball.ys = 0.0;
	cfg->state.ball.xd = 1.0;
	cfg->state.ball.yd = 1.0;
	cfg->state.ball.w = 1.0;
	cfg->state.ball.h = 1.0;
	cfg->state.ball.clast = NULL;
	cfg->state.pleft.x = 0.05;
	cfg->state.pleft.y = 0.5;
	cfg->state.pleft.w = 0.05;
	cfg->state.pleft.h = 0.2;
	cfg->state.pleft.points = 0;
	cfg->state.pright.x = 1.0 - cfg->state.pleft.x;
	cfg->state.pright.y = 1.0 - cfg->state.pleft.y;
	cfg->state.pright.w = cfg->state.pleft.w;
	cfg->state.pright.h = cfg->state.pleft.h;
	cfg->state.pright.points = 0;
	renderinit(cfg);
}

#define MAGICDT 0.005
void update(struct gnop_cfg *cfg) {
	SDL_Event e;
	const Uint8 *state = SDL_GetKeyboardState(NULL);
	double dt, rdt;
	static double accum = 0.0;
	cfg->state.ctick = gettime();
	rdt = cfg->state.ctick - cfg->state.ltick;
	SDL_GL_GetDrawableSize(cfg->win, &(cfg->state.w), &(cfg->state.h));
	cfg->state.ball.w = 0.05 * (double) ((double) cfg->state.h / (double) cfg->state.w);
	cfg->state.ball.h = cfg->state.ball.w * (double) ((double) cfg->state.w / (double) cfg->state.h);
	while (SDL_PollEvent(&e) != 0) {
		switch (e.type) {
		case SDL_QUIT:
			cfg->state.alive = SDL_FALSE;
			break;
		case SDL_KEYDOWN:
			switch (e.key.keysym.sym) {
			case 'q':
			case SDLK_ESCAPE:
				cfg->state.alive = SDL_FALSE;
				break;
			case 'v':
				cfg->state.vsync = !cfg->state.vsync;
				SDL_DestroyRenderer(cfg->ctx);
				cfg->ctx = NULL;
				renderinit(cfg);
				break;
			case ' ':
			case 'p':
				cfg->state.paused = !cfg->state.paused;
				break;
			case 'r':
				gameinit(cfg);
				break;
			case '+':
				cfg->state.speed += 0.05;
				printf("speed: %.2f\n", cfg->state.speed);
				break;
			case '-':
				cfg->state.speed -= 0.05;
				printf("speed: %.2f\n", cfg->state.speed);
				break;
			}
		}
	}
	if (rdt + accum < MAGICDT) {
		return;
	}
	if (cfg->state.paused == SDL_TRUE) {
		cfg->state.ltick = cfg->state.ctick;
		return;
	}
	accum += rdt;
	while (accum >= MAGICDT) {
		dt = MAGICDT;
		/* left paddle */
		if (state[SDL_SCANCODE_W]) {
			cfg->state.pleft.y += dt * 1.4 * cfg->state.speed;
		}
		if (state[SDL_SCANCODE_S]) {
			cfg->state.pleft.y -= dt * 1.4 * cfg->state.speed;
		}
		/* right paddle */
		if (state[SDL_SCANCODE_UP]) {
			cfg->state.pright.y += dt * 1.4 * cfg->state.speed;
		}
		if (state[SDL_SCANCODE_DOWN]) {
			cfg->state.pright.y -= dt * 1.4 * cfg->state.speed;
		}
		/* clamp y */
		cfg->state.pleft.y = clamp(
			cfg->state.pleft.y,
			0.0,
			1.0
		);
		cfg->state.pright.y = clamp(
			cfg->state.pright.y,
			0.0,
			1.0
		);
		/* ball! */
		/*
		cfg->state.ball.xs = 0.1 * (0.1 + cfg->state.pleft.y) / 1.1;
		cfg->state.ball.ys = 0.1 * (0.1 + cfg->state.pright.y) / 1.1;
		*/
		/* x-treme speed :O */
		{
			double diffx, diffy, diffz;
			diffx = cfg->state.pleft.x - cfg->state.ball.x;
			diffy = cfg->state.pleft.y - cfg->state.ball.y;
			diffz = cfg->state.pright.x - cfg->state.pright.y;
			cfg->state.ball.xs = SDL_sqrt(
				diffx * diffx +
				diffy * diffy +
				diffz * diffz
			);
		}
		/* y-mazing speed :O */
		{
			double diffx, diffy, diffz;
			diffx = cfg->state.pright.x - cfg->state.ball.x;
			diffy = cfg->state.pright.y - cfg->state.ball.y;
			diffz = cfg->state.pleft.x - cfg->state.pleft.y;
			cfg->state.ball.ys = SDL_sqrt(
				diffx * diffx +
				diffy * diffy +
				diffz * diffz
			);
		}
		cfg->state.ball.x += dt * cfg->state.speed * cfg->state.ball.xd * 0.9 * cfg->state.ball.xs;
		cfg->state.ball.y += dt * cfg->state.speed * cfg->state.ball.yd * 0.9 * cfg->state.ball.ys;
		cfg->state.ball.x = clamp(cfg->state.ball.x, 0.0, 1.0);
		cfg->state.ball.y = clamp(cfg->state.ball.y, 0.0, 1.0);
		/* paddle collision */
		collide(&(cfg->state.pleft), &(cfg->state.ball));
		collide(&(cfg->state.pright), &(cfg->state.ball));
		/* edge collision */
		if (cfg->state.ball.x == 0.0 || cfg->state.ball.x == 1.0) {
			double tmp;
			cfg->state.ball.clast = NULL;
			cfg->state.ball.xd = - cfg->state.ball.xd;
			tmp = cfg->state.ball.xs;
			cfg->state.ball.xs = cfg->state.ball.ys;
			cfg->state.ball.ys = tmp;
			if (cfg->state.ball.x < 0.5) {
				cfg->state.pright.points += 1;
			} else {
				cfg->state.pleft.points += 1;
			}
			cfg->state.ball.x = 0.5;
			cfg->state.ball.y = 0.5;
			if (
				cfg->state.pleft.points == 10 ||
				cfg->state.pright.points == 10
			) {
				printf(
					"Score reset in favor of %c!\n",
					(cfg->state.pleft.points == 10) ? 'L' : 'R'
				);
				cfg->state.lwin = (cfg->state.pleft.points == 10) ? 10 : 11;
				cfg->state.pleft.points = 0;
				cfg->state.pright.points = 0;
			} else {
				printf("L: %d R: %d\n", cfg->state.pleft.points, cfg->state.pright.points);
			}
		}
		if (cfg->state.ball.y == 0.0 || cfg->state.ball.y == 1.0) {
			double tmp;
			cfg->state.ball.clast = NULL;
			cfg->state.ball.yd = - cfg->state.ball.yd;
			tmp = cfg->state.ball.ys;
			cfg->state.ball.ys = cfg->state.ball.xs;
			cfg->state.ball.xs = tmp;
		}
		/* decrement accumulator */
		accum -= dt;
	}
	/* bump the tick */
	cfg->state.ltick = cfg->state.ctick;
}

void render(struct gnop_cfg *cfg) {
	SDL_Rect box;
	/* clear */
	SDL_SetRenderDrawColor(cfg->ctx, 0, 127, 255, 255);
	SDL_RenderClear(cfg->ctx);
	SDL_SetRenderDrawBlendMode(cfg->ctx, SDL_BLENDMODE_BLEND);
	/* left paddle */
	SDL_SetRenderDrawColor(cfg->ctx, 255, 42, 42, 255);
	box.w = cfg->state.w * cfg->state.pleft.w;
	box.h = cfg->state.h * cfg->state.pleft.h;
	box.x = cfg->state.w * cfg->state.pleft.x - (box.w / 2);
	box.y = cfg->state.h * (1.0 - cfg->state.pleft.y) - (box.h / 2);
	SDL_RenderFillRect(cfg->ctx, &box);
	drawnum(cfg, cfg->state.pleft.points, 0.3, 0.1, 0.05);
	/* right paddle */
	SDL_SetRenderDrawColor(cfg->ctx, 42, 255, 42, 255);
	box.w = cfg->state.w * cfg->state.pright.w;
	box.h = cfg->state.h * cfg->state.pright.h;
	box.x = cfg->state.w * cfg->state.pright.x - (box.w / 2);
	box.y = cfg->state.h * (1.0 - cfg->state.pright.y) - (box.h / 2);
	drawnum(cfg, cfg->state.pright.points, 0.7, 0.1, 0.05);
	SDL_RenderFillRect(cfg->ctx, &box);
	/* last winner */
	SDL_SetRenderDrawColor(cfg->ctx, 42, 42, 255, 255);
	if (cfg->state.lwin == 10) {
		SDL_SetRenderDrawColor(cfg->ctx, 255, 42, 42, 255);
	} else if (cfg->state.lwin == 11) {
		SDL_SetRenderDrawColor(cfg->ctx, 42, 255, 42, 255);
	}
	drawnum(cfg, (int) cfg->state.lwin, 0.5, 0.1, 0.07);
	/* ball */
	SDL_SetRenderDrawColor(cfg->ctx, 255, 255, 255, 255);
	box.w = cfg->state.w * cfg->state.ball.w;
	box.h = cfg->state.h * cfg->state.ball.h;
	box.x = cfg->state.w * cfg->state.ball.x - (box.w / 2);
	box.y = cfg->state.h * (1.0 - cfg->state.ball.y) - (box.h / 2);
	SDL_RenderFillRect(cfg->ctx, &box);
	/* pause screen */
	if (cfg->state.paused) {
		SDL_SetRenderDrawColor(cfg->ctx, 205, 205, 205, 127);
		box.w = cfg->state.w;
		box.h = cfg->state.h;
		box.x = 0;
		box.y = 0;
		SDL_RenderFillRect(cfg->ctx, &box);
		SDL_SetRenderDrawColor(cfg->ctx, 255, 127, 42, 127);
		drawnum(cfg, 13, 0.5, 0.5, 0.2);
	}
	/* ship it */
	SDL_RenderPresent(cfg->ctx);
}

void gameloop(struct gnop_cfg *cfg) {
	while (cfg->state.alive == SDL_TRUE) {
		update(cfg);
		render(cfg);
	}
}

int main(int argc, char **argv) {
	struct gnop_cfg cfg;
	cfg.win = NULL;
	cfg.ctx = NULL;
	cfg.state.w = 1280;
	cfg.state.h = 720;
	(void) argc;
	(void) argv;
	SDL_assert_release(
		SDL_Init(SDL_INIT_EVERYTHING) == 0
	);
	cfg.state.alive = SDL_TRUE;
	gameinit(&cfg);
	gameloop(&cfg);
	SDL_Quit();
	return 0;
}
