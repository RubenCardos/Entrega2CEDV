/* **********************************************************
** Importador NoEscape 1.0
** Curso de Experto en Desarrollo de Videojuegos 
** Escuela Superior de Informatica - Univ. Castilla-La Mancha
** Carlos Gonzalez Morcillo - David Vallejo Fernandez
************************************************************/

#ifndef SCENE_H
#define SCENE_H

#include <vector>
#include <Camera.h>
#include <Node.h>
#include <Graph.h>

class Scene
{
 public:
  Scene ();
  ~Scene ();
  
  void addCamera (Camera* camera);    
  Graph* getGraph () const { return _graph;}
  std::vector<Camera*> getCameras () const { return _cameras; }
    
 private:
  Graph *_graph;
  std::vector<Camera*> _cameras;
};

#endif
