#include <Scene.h>

Pacman::Scene::Scene ()
{
  _graph = new Pacman::Graph;
}

Pacman::Scene::~Scene ()
{
}

void
Pacman::Scene::addCamera
(Pacman::Camera* camera)
{
  _cameras.push_back(camera);
}
