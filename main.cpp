#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <vector>
#include <cmath>

#define Width 800
#define Height 600
#define Cube_Size 70

double pi = std::acos(-1.0);

Uint64 frameStart;
int frameTime;

const int FPS = 60;
const int FrameDelay = 1000 / FPS;

int form = 0; // 0 = cube, 1 = pyramid
std::vector<std::vector<float>> points;
std::vector<std::vector<int>> edges;


std::vector<std::vector<float>> Cube_Points = {
    { 1, 1, 1 },
    { 1, 1, -1},
    { 1, -1, 1},
    { -1, 1, 1},
    { 1, -1, -1},
    {-1, 1, -1},
    {-1, -1, 1},
    {-1, -1, -1}
};

std::vector<std::vector<int>> Cube_Edges = {
    { 0, 1 },
    { 0, 2 },
    { 0, 3 },
    { 1, 4 },
    { 1, 5 },
    { 2, 4 },
    { 2, 6 },
    { 3, 5 },
    { 3, 6 },
    { 4, 7 },
    { 5, 7 },
    { 6, 7 }
};

std::vector<std::vector<float>> pyramidPoints = {
    { 0, 1, 0 },
    { -1, -1, 1 },
    { 1, -1, 1 },
    { 1, -1, -1 },
    { -1, -1, -1 }
};

std::vector<std::vector<int>> pyramidEdges = {
    { 0, 1 },
    { 0, 2 },
    { 0, 3 },
    { 0, 4 },
    { 1, 2 },
    { 2, 3 },
    { 3, 4 },
    { 4, 1 }
};

std::vector<double> RotatePoint(double x, double y, int degrees) {
    double radius = std::sqrt(x * x + y * y);

	if (radius == 0) {
		return { x, y };
	}
    
    double currentAngle = std::atan2(y, x) * (180 / pi);
    double newAngle = currentAngle + degrees;
    double radiant = newAngle * (pi / 180);

    double newX = radius * std::cos(radiant);
    double NewY = radius * std::sin(radiant);

    return { newX, NewY };
}

std::vector<int> Get2dPointsFrom3d(double x, double y, double z, float StartX, float StartY, float StartZ, int AngleX, int AngleY, int AngleZ) {
    int X, Y;
    y = -y;

    std::vector<double> RotatedX = RotatePoint(y, z, AngleX);
    y = RotatedX[0]; z = RotatedX[1];

    std::vector<double> RotatedY = RotatePoint(z, x, AngleY);
    z = RotatedY[0]; x = RotatedY[1];

    std::vector<double> RotatedZ = RotatePoint(x, y, AngleZ);
    x = RotatedZ[0]; y = RotatedZ[1];

    double cameraZ = 3.0 * StartZ;
    double depth = cameraZ + z;
    if (depth < 0.1) depth = 0.1;

    double transX = x - StartX;
    double transY = y - StartY;

    double fov = 500;
    X = static_cast<int>((transX * fov) / depth) + Width / 2;
    Y = static_cast<int>((transY * fov) / depth) + Height / 2;

    return { X, Y };
}


int main(int argc, char* argv[]) {
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_Log("SDL_Init Error: %s", SDL_GetError());
        return 1;
    }

    SDL_Window* window = NULL;
    SDL_Renderer* renderer = NULL;

    if (!SDL_CreateWindowAndRenderer("3D Renderer", Width, Height, 0, &window, &renderer)) {
        SDL_Log("SDL window and renderer creating error ocupied! ");
        SDL_Quit();
        return 1;
    }

    SDL_Texture* texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_STREAMING, Width, Height);
    if (!texture) {
        SDL_Log("Window Write Stream could not be created! ");
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    bool running = true;
    SDL_Event e;
	float posX = 0, posY = 0, posZ = 1, rotX = 0, rotY = 0, rotZ = 0;
    while (running) {
		frameStart = SDL_GetTicks();

        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_EVENT_QUIT) {
                running = false;
            }

            if (e.type == SDL_EVENT_KEY_DOWN) {
                switch (e.key.key) {
					case SDLK_ESCAPE:
						running = false;
						break;

                }
            }
        }


        // Moving
        int numkeys;
		const bool* state = SDL_GetKeyboardState(&numkeys);

        
        if (state[SDL_SCANCODE_LSHIFT]) posY -= 0.1;
        if (state[SDL_SCANCODE_SPACE]) posY += 0.1;
        if (state[SDL_SCANCODE_A]) posX -= 0.1;
        if (state[SDL_SCANCODE_D]) posX += 0.1;
        if (state[SDL_SCANCODE_W]) posZ -= 0.01;
        if (state[SDL_SCANCODE_S]) posZ += 0.01;
		if (state[SDL_SCANCODE_UP]) rotX += 1;
		if (state[SDL_SCANCODE_DOWN]) rotX -= 1;
		if (state[SDL_SCANCODE_LEFT]) rotY += 1;
		if (state[SDL_SCANCODE_RIGHT]) rotY -= 1;
		if (state[SDL_SCANCODE_Q]) rotZ += 1;
		if (state[SDL_SCANCODE_E]) rotZ -= 1;
        if (state[SDL_SCANCODE_1]) form = 0;
        if (state[SDL_SCANCODE_2]) form = 1;

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
        
		SDL_SetRenderDrawColor(renderer, 0, 255, 255, 255);

        std::vector<std::vector<int>> Points;

		int RotationAngle = (SDL_GetTicks() / 10) % 360;

        if (form == 0){
            points = Cube_Points;
            edges = Cube_Edges;
        }else if (form == 1){
            points = pyramidPoints;
            edges = pyramidEdges;
        }

        for (int i = 0; i < points.size(); i++) {
            Points.push_back(Get2dPointsFrom3d(points[i][0], points[i][1], points[i][2], posX, posY, posZ, rotX, rotY, rotZ));
        }
        for (int i = 0; i < edges.size(); i++) {
			SDL_RenderLine(renderer, (int)Points[edges[i][0]][0], (int)Points[edges[i][0]][1], (int)Points[edges[i][1]][0], (int)Points[edges[i][1]][1]);
        }

        SDL_RenderPresent(renderer);

		frameStart = SDL_GetTicks() - frameStart;

        if (FrameDelay > frameStart) {
            SDL_Delay(FrameDelay - frameStart);
        }
    }
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}