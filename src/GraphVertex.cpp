/* **********************************************************
** Importador NoEscape 1.0
** Curso de Experto en Desarrollo de Videojuegos 
** Escuela Superior de Informatica - Univ. Castilla-La Mancha
** Carlos Gonzalez Morcillo - David Vallejo Fernandez
************************************************************/

#include <GraphVertex.h>

Pacman::GraphVertex::GraphVertex
(const Pacman::Node& data)
  :_data(data)
{
}

Pacman::GraphVertex::~GraphVertex ()
{
  std::vector<Pacman::GraphEdge*>::iterator itEdge; 
  for (itEdge = _edges.begin();
       itEdge != _edges.end();
       ++itEdge)
    delete *itEdge;
  
  _edges.clear();
}

std::vector<Pacman::GraphVertex*>
Pacman::GraphVertex::adjacents () const
{
  std::vector<Pacman::GraphVertex*> result;
  std::vector<Pacman::GraphEdge*>::const_iterator it;

  for (it = _edges.begin();
       it != _edges.end();
       ++it)
    result.push_back((*it)->getDestination());

  return result;
}
