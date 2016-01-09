/* **********************************************************
** Importador NoEscape 1.0
** Curso de Experto en Desarrollo de Videojuegos 
** Escuela Superior de Informatica - Univ. Castilla-La Mancha
** Carlos Gonzalez Morcillo - David Vallejo Fernandez
************************************************************/

#include <GraphEdge.h>

Pacman::GraphEdge::GraphEdge
(Pacman::GraphVertex* origin, Pacman::GraphVertex* dest)
  :_pOrigin(origin), _pDestination(dest)
{
}

Pacman::GraphEdge::~GraphEdge () 
{
}
