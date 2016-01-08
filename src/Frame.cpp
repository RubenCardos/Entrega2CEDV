/* **********************************************************
** Importador NoEscape 1.0
** Curso de Experto en Desarrollo de Videojuegos 
** Escuela Superior de Informatica - Univ. Castilla-La Mancha
** Carlos Gonzalez Morcillo - David Vallejo Fernandez
************************************************************/

#include <Frame.h>

Frame::Frame
(int index, const Ogre::Vector3& position, const Ogre::Vector4& rotation):
  _index(index), _position(position), _rotation(rotation)
{
}

Frame::~Frame ()
{
}
