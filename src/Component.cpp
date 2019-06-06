#include "Component.h"

Component* Component::Create() {
   fprintf(stderr, "Error: Create() called from base class\n");
}
