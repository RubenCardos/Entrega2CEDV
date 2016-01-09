/* **********************************************************
** Importador NoEscape 1.0
** Curso de Experto en Desarrollo de Videojuegos 
** Escuela Superior de Informatica - Univ. Castilla-La Mancha
** Carlos Gonzalez Morcillo - David Vallejo Fernandez
************************************************************/

#include <Graph.h>

Pacman::Graph::Graph ()
{
  // Reserva de memoria inicial.
  _vertexes.reserve(25);
  _edges.reserve(25);
}

Pacman::Graph::~Graph ()
{
  // Liberar vértices.
  typename std::vector<GraphVertex*>::iterator itVert;
  for (itVert = _vertexes.begin();
       itVert != _vertexes.end();
       ++itVert)
    delete *itVert;

  // Liberar arcos.
  typename std::vector<GraphEdge*>::iterator itEdge;
  for (itEdge = _edges.begin();
       itEdge != _edges.end();
       ++itEdge)
    delete *itEdge;

  _vertexes.clear();
  _edges.clear();
}

void
Pacman::Graph::addVertex
(GraphVertex* pVertex)
{
  _vertexes.push_back(pVertex);
}

void
Pacman::Graph::addEdge
(Pacman::GraphVertex* pOrigin, Pacman::GraphVertex* pDestination, bool undirected)
{
  Pacman::GraphEdge* pEdge = new GraphEdge(pOrigin, pDestination);
  _edges.push_back(pEdge);
  pOrigin->addEdge(pEdge);
    
  if (undirected) {
    Pacman::GraphEdge* pEdge2 = new GraphEdge(pDestination, pOrigin);
  _edges.push_back(pEdge2);
    pDestination->addEdge(pEdge2);
  }
}

std::vector<Pacman::GraphVertex*>
Pacman::Graph::adjacents
(int index) const
{
  std::vector<Pacman::GraphVertex*> result;
  std::vector<Pacman::GraphVertex*>::const_iterator it;

  for (it = _vertexes.begin();
       it != _vertexes.end();
       ++it)
    if ((*it)->getData().getIndex() == index) 
      return (*it)->adjacents();

  return result;
}

Pacman::GraphVertex*
Pacman::Graph::getVertex
(int index) const
{
  std::vector<Pacman::GraphVertex*>::const_iterator it;

  for (it = _vertexes.begin();
       it != _vertexes.end();
       ++it)
    if ((*it)->getData().getIndex() == index) 
      return (*it);

  return NULL;
}
