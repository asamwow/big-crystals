#include "Material.h"

Type Material::getType() { return Type::Material; }

class Material *Material::defaultMaterial;
class Material *Material::crystalMaterial;
