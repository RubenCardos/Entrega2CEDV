/* **********************************************************
** Importador NoEscape 1.0
** Curso de Experto en Desarrollo de Videojuegos 
** Escuela Superior de Informatica - Univ. Castilla-La Mancha
** Carlos Gonzalez Morcillo - David Vallejo Fernandez
************************************************************/

#include <Camera.h>

Camera::Camera 
(int index, int fps):
  _index(index), _fps(fps)
{
}

Camera::~Camera ()
{
  // Liberar el path.
  typename std::vector<Frame*>::iterator it;
  for (it = _path.begin();
       it != _path.end();
       ++it)
    delete *it;
}

void
Camera::addFrameToPath
(Frame* frame)
{
  _path.push_back(frame);
}

Camera::operator
std::string() const
{
  std::stringstream r;
  r << "[Camera: " << _index << " FPS: " << _fps << "]";
  return r.str();
}
