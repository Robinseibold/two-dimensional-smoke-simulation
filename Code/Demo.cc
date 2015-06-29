#include <iostream>
#include <SDL2/SDL.h>
#include <openGL/gl.h>

#include "Fluid.h"

#define FLUID_SIZE      (128)                           /* Size of the fluid                                */
#define WINDOW_SIZE     (512)                           /* Size of the SDL window                           */
#define DT              (0.1f)                          /* Abstract time between each fluid iteration       */
#define PARTICLE_AMOUNT (50.0f)                         /* The number of particles to add                   */
#define FORCE           (3.0f)                          /* The force of the velocity to add                 */
#define DIFFUSION       (0.000001f)                     /* Diffusion rate                                   */
#define VISCOSITY       (0.000001f)                     /* Fluid viscosity                                  */

SDL_GLContext context;                                  /* openGL context                                   */
SDL_Window *window;                                     /* SDL window                                       */

void initSDL(void) {
    if(SDL_Init(SDL_INIT_VIDEO) != 0) {
        cout << "Could not initilize SDL: " << SDL_GetError() << endl;
    }
    
    SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );
    window = SDL_CreateWindow("Two-Dimensional Smoke Simulation",
                              SDL_WINDOWPOS_UNDEFINED,
                              SDL_WINDOWPOS_UNDEFINED,
                              WINDOW_SIZE, WINDOW_SIZE,
                              SDL_WINDOW_OPENGL);
    if(!window) {
        cout << "Could not initilize window: " << SDL_GetError() << endl;
    }
    
    context = SDL_GL_CreateContext(window);
    if(!context) {
        cout << "Could not initilize context: " << SDL_GetError() << endl;
    }
    SDL_GL_SetSwapInterval(1);
}

void initOpenGL(void) {
    glEnable(GL_TEXTURE_2D);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glViewport(0, 0, WINDOW_SIZE, WINDOW_SIZE);
    glClear(GL_COLOR_BUFFER_BIT);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0f, WINDOW_SIZE, WINDOW_SIZE, 0.0f, -1.0f, 1.0f);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

int main(int argc, char **argv){
    initSDL();                                          /* Initilize SDL                                    */
    initOpenGL();                                       /* Initilize openGL                                 */
    
    SDL_Event event;                                    /* Variable which stores events                     */
    bool active = true;                                 /* Loop variable, main loop while true              */
    bool mouseButtonDown[] = {false, false};            /* Variables to keep track of mouseButton states    */
    bool mouseMove = false;                             /* Variable to keep track of possible mouse motion  */
    int startX, startY, currentX, currentY;             /* Variables used for mouse position                */
    
    Fluid fluid(FLUID_SIZE);                            /* Initilize the fluid                              */
    
    while (active) {
        mouseMove = false;
        while(SDL_PollEvent(&event)) {
            switch(event.type) {
                case SDL_QUIT:
                    active = false;
                    break;
                case SDL_MOUSEBUTTONDOWN:
                    if(event.button.button == SDL_BUTTON_LEFT && !mouseButtonDown[0]) {
                        mouseButtonDown[0] = true;
                    }else if (event.button.button == SDL_BUTTON_RIGHT && !mouseButtonDown[1]) {
                        mouseButtonDown[1] = true;
                    }
                    SDL_GetMouseState(&startX, &startY);
                    currentX = startX;
                    currentY = startY;
                    break;
                case SDL_MOUSEBUTTONUP:
                    if(event.button.button == SDL_BUTTON_LEFT && mouseButtonDown[0]) {
                        mouseButtonDown[0] = false;
                    }else if(event.button.button == SDL_BUTTON_RIGHT && mouseButtonDown[1]) {
                        mouseButtonDown[1] = false;
                    }
                case SDL_MOUSEMOTION:
                    if(mouseButtonDown[0] || mouseButtonDown[1]) {
                        SDL_GetMouseState(&currentX, &currentY);
                        mouseMove = true;
                    }
                    break;
                case SDL_KEYDOWN:
                    switch(event.key.keysym.sym) {
                        case SDLK_ESCAPE:
                            active = false;
                            break;
                        case SDLK_SPACE:
                            fluid.clearPrevious();
                            fluid.clearCurrent();
                            break;
                        default: break;
                    }
                    break;
                default: break;
            }
        }
        fluid.clearPrevious();
        if(mouseMove) {
            int i = int (((float (currentX)) / WINDOW_SIZE) * FLUID_SIZE);
            int j = int (((float (currentY)) / WINDOW_SIZE) * FLUID_SIZE);
            if((i > 0) && (i < (FLUID_SIZE - 1)) && (j > 0) && (j < (FLUID_SIZE - 1))) {
                if (mouseButtonDown[0]) fluid.addSourceDensity(PARTICLE_AMOUNT, i, j);
                fluid.addSourceVelocity(FORCE, (currentX - startX), (currentY - startY), i, j);
            }
            startX = currentX;
            startY = currentY;
        }
        fluid.velocityStep(VISCOSITY, DT);
        fluid.densityStep(DIFFUSION, DT);
        auto density = fluid.getDensity();
        glClear(GL_COLOR_BUFFER_BIT);
        glBegin(GL_QUADS);
        float unit = (float (WINDOW_SIZE) / (FLUID_SIZE - 1));
        for(int i = 0; i != (FLUID_SIZE - 1); ++i) {
            float x = i * unit;
            for(int j = 0; j != (FLUID_SIZE - 1); ++j) {
                float y = j * unit;
                float densityColorOne = density[i][j];
                float densityColorTwo = density[i + 1][j];
                float densityColorThree = density[i + 1][j + 1];
                float densityColorFour = density[i][j + 1];
                glColor3f(densityColorOne, densityColorOne, densityColorOne);
                glVertex3f(x, y, 0);
                glColor3f(densityColorTwo, densityColorTwo, densityColorTwo);
                glVertex3f((x + unit), y, 0);
                glColor3f(densityColorThree, densityColorThree, densityColorThree);
                glVertex3f((x + unit), (y + unit), 0);
                glColor3f(densityColorFour, densityColorFour, densityColorFour);
                glVertex3f(x, (y + unit), 0);
            }
        }
        glEnd();
        SDL_GL_SwapWindow(window);
    }
    SDL_GL_DeleteContext(context);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 1;
}


















