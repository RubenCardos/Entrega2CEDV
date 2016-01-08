/* **********************************************************
** Importador NoEscape 1.0
** Curso de Experto en Desarrollo de Videojuegos 
** Escuela Superior de Informatica - Univ. Castilla-La Mancha
** Carlos Gonzalez Morcillo - David Vallejo Fernandez
************************************************************/

#include <Node.h>

Node::Node ()
{
}

Node::Node
(const int& index, const string& type, const Ogre::Vector3& position)
{
  _index = index;
  _type = type;
  _position = position;
}

Node::~Node ()
{
}

Node::operator
std::string() const
{
  std::stringstream r;
  r << "[Node: " << _index << " Type: " << _type << " (" 
    << _position.x << ", " << _position.y << ", " 
    << _position.z << ")]";
  return r.str();
}
