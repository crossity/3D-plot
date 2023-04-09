#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <utility>

#include <SDL2/SDL.h>

using namespace std;

#define Width 400
#define Height 300

#define step 0.01f

#define x_size 2.f
#define y_size 2.f
#define z_size 2.f

float aspect = (float)Width / (float)Height;

struct vec3 {
    float x, y, z;
    float r, g, b;

    vec3(float a, float b, float c, float d = 0.f, float e = 0.f, float f = 0.f) {
        this->x = a;
        this->y = b;
        this->z = c;
        this->r = d;
        this->g = e;
        this->b = f;
    }
    vec3() {
        this->x = 0.f;
        this->y = 0.f;
        this->z = 0.f;
        this->r = 0.f;
        this->g = 0.f;
        this->b = 0.f;
    }

    vec3 operator-(vec3 a) {
        return vec3(this->x - a.x, this->y - a.y, this->z - a.z, this->r, this->g, this->b);
    }
};

struct vec2 {
    float x, y;
    float r, g, b;

    vec2(float a, float b, float c = 0.f, float d = 0.f, float e = 0.f) {
        this->x = a;
        this->y = b;
        this->r = c;
        this->g = d;
        this->b = e;
    }
    vec2() {
        this->x = 0.f;
        this->y = 0.f;
        this->r = 0.f;
        this->g = 0.f;
        this->b = 0.f;
    }
};

vec3 camera_pos;
vec3 camera_angle;

vec2 render(vec3 p);
vec3 to_camera(vec3 p);

vec3 rotate_y(vec3 c, float angle, vec3 p);
vec3 rotate_x(vec3 c, float angle, vec3 p);

vector<vec3> z_sort(vector<vec3> figure);

int main(int argc, char *argv[]) {
    SDL_Init(SDL_INIT_EVERYTHING);

    SDL_Window *window = SDL_CreateWindow("figure 1", 100, 100, Width, Height, SDL_WINDOW_ALLOW_HIGHDPI);
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    if (window == NULL) {
        cout << "Window instializing error" << endl;
        return 1;
    }

    vector<vec3> figure;

    for (float x = -x_size; x <= x_size; x += step) {
        for (float z = -z_size; z <= z_size; z += step) {
            float y = sin(x) * sin(z);
            figure.push_back(vec3(x, y, z, (x + x_size) / 2 / x_size * 255, (y + y_size) / 2 / y_size * 255, (z + z_size) / 2 / z_size * 255));
        }
    }

    camera_pos = vec3(-x_size - 0.3f, 2.f, -z_size - 0.3f);
    camera_angle = vec3(-0.8f, 3.14f / 4 + 0.1f, 0.f);

    SDL_Event event;

    bool run = true;

    while (run) {
        while(SDL_PollEvent(&event)){
            switch(event.type) {
                case SDL_QUIT:
                    run = false;
                    break;
                default:
                    break;
            }
        }

        vector<vec3> sorted_figure = z_sort(figure);
        vector<vec2> rendered_figure;

        for (unsigned int i = 0; i < sorted_figure.size(); i++) {
            if (to_camera(sorted_figure[i]).z > 0)
                rendered_figure.push_back(render(sorted_figure[i]));
        }

        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 0);
        SDL_RenderClear(renderer);

        for (unsigned int i = 0; i < rendered_figure.size(); i++) {
            SDL_SetRenderDrawColor(renderer, (int)rendered_figure[i].r, (int)rendered_figure[i].g, (int)rendered_figure[i].b, 0);
            SDL_RenderDrawPoint(renderer, (int)rendered_figure[i].x, (int)rendered_figure[i].y);
        }

        SDL_RenderPresent(renderer);
    }

    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}

vec2 render(vec3 p) {
    p = to_camera(p);
    if (p.z == 0) {
        p.z = 0.0001f;
    }
    return vec2(Width * (p.x / aspect + 0.75f * p.z) / (1.5f * p.z), Height * (0.75f * p.z - p.y) / (1.5f * p.z), p.r, p.g, p.b);
}

vec3 to_camera(vec3 p) {
    p = rotate_y(camera_pos, -camera_angle.y, p);
    p = rotate_x(camera_pos, -camera_angle.x, p);
    p = p - camera_pos;
    return p;
}

vec3 rotate_y(vec3 c, float angle, vec3 p) {
    float sn = sin(angle);
    float cs = cos(angle);

    p.z -= c.z;
    p.x -= c.x;

    float znew = p.z * cs - p.x * sn;
    float xnew = p.z * sn + p.x * cs;

    p.x = xnew + c.x;
    p.z = znew + c.z;
    return p;
}

vec3 rotate_x(vec3 c, float angle, vec3 p) {
    float sn = sin(angle);
    float cs = cos(angle);

    p.z -= c.z;
    p.y -= c.y;

    float znew = p.z * cs - p.y * sn;
    float ynew = p.z * sn + p.y * cs;

    p.y = ynew + c.y;
    p.z = znew + c.z;
    return p;
}

vector<vec3> z_sort(vector<vec3> figure) {
    vector<pair<float, int>> tmp(figure.size());
    vector<vec3> ret(figure.size());

    for (unsigned int i = 0; i < figure.size(); i++) {
        tmp[i].first = to_camera(figure[i]).z;
        tmp[i].second = i;
    }
    sort(tmp.begin(), tmp.end());

    for (unsigned int i = 0; i < figure.size(); i++)
        ret[figure.size() - 1 - i] = figure[tmp[i].second];

    return ret;
}