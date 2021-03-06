#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <sys/time.h>

#include <vector>

#include "SDL/SDL.h"
#include "FrameBufferSDL.h"
#include "VideoSystemSDL.h"
#include "Gene.h"
#include "TspCity.h"
#include "GeneticTsp.h"
#include "AStar.h"

using namespace poppy;

const int kScreenWidth    = 640;
const int kScreenHeight   = 480;

void drawTspLine(VideoSystem& videosys, const std::vector<TspCity>& cities,
                 const std::vector<int>& seq)
{
  for (size_t i = 0; i < seq.size(); i++)
  {
    size_t prev = i;
    size_t next = (i + 1) % seq.size();

    prev = seq[prev];
    next = seq[next];
    videosys.drawLine(cities[prev].x(), cities[prev].y(), cities[next].x(),
                      cities[next].y(), 0xff);
  }
}

int main(int argc, char *argv[])
{
  int width = 640;
  int height = 480;

  ::srand(static_cast<unsigned int>(::time(NULL)));

  std::vector<std::vector<bool> > legal {
    std::vector<bool> {0, 0, 0, 1, 1, 1, 1},
    std::vector<bool> {1, 1, 1, 1, 0, 1, 0},
    std::vector<bool> {1, 1, 1, 1, 1, 0, 0},
    std::vector<bool> {1, 0, 0, 0, 0, 0, 0},
    std::vector<bool> {1, 1, 1, 1, 1, 1, 1},
  };
  Point start(1, 5);
  Point end(4, 5);
  AStar *astar = new AStar(legal, start, end);
  astar->getDistance();
  astar->getPath();

  std::vector<TspCity> cities;
  for (int i = 0; i < 16; i++)
  {
    int x = ::rand() % kScreenWidth;
    int y = ::rand() % kScreenHeight;

    cities.push_back(TspCity(x, y));
  }

  GeneticTsp tsp(cities, 1000, 0.05, 0.5);
  tsp.InitGroup();

  VideoSystemSDL videoSys;
  videoSys.createWindow(width, height, 32, false);

  SDL_Event event;
  while (true)
  {
    videoSys.fillSecondary(0x0);

    videoSys.lockSecondary();
    drawTspLine(videoSys, cities, tsp.bestGene().sequenceOfIdx());
    videoSys.unlockSecondary();

    videoSys.flipDisplay();

    tsp.evolution();

    if (SDL_PollEvent(&event))
    {
      if (event.type == SDL_QUIT)
      {
        break;
      }
    }
  }

  return 0;
}

