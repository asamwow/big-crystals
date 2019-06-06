#include "Mesh.h"

Type Mesh::getType() { return Type::Mesh; }

using namespace std;

class Mesh *Mesh::crystalMesh;

// set object mesh data manually
void Mesh::SetData(std::vector<unsigned int> indices,
                   std::vector<float> vertices, std::vector<float> colors) {
   this->indices = indices;
   this->vertices = vertices;
   this->colors = colors;   
   init();
}
void Mesh::SetData(unsigned int *newIndices, int indiceCount,
                   float *newVertices, int verticeCount, float *newColors,
                   int colorsCount) {
   // convert array to vector
   SetData(std::vector<unsigned int>(newIndices, newIndices + indiceCount),
           std::vector<float>(newVertices, newVertices + verticeCount),
           std::vector<float>(newColors, newColors + colorsCount));
   free(newIndices);
   free(newVertices);
   free(newColors);
}

void Mesh::SetPrimitiveSphere(int stackCount, int sectorCount, float radius,
                              glm::vec3 color, char shade) {
   std::vector<float> vertices;
   std::vector<float> colors;
   // std::vector<float> normals;

   float x, y, z, xy;                           // vertex position
   float nx, ny, nz, lengthInv = 1.0f / radius; // vertex normal

   float sectorStep = 2 * PI / sectorCount;
   float stackStep = PI / stackCount;
   float sectorAngle, stackAngle;

   for (int i = 0; i <= stackCount; ++i) {
      stackAngle = PI / 2 - i * stackStep; // starting from pi/2 to -pi/2
      xy = radius * cosf(stackAngle);      // r * cos(u)
      z = radius * sinf(stackAngle);       // r * sin(u)

      // add (sectorCount+1) vertices per stack
      // the first and last vertices have same position and normal, but
      // different tex coords
      for (int j = 0; j <= sectorCount; ++j) {
         sectorAngle = j * sectorStep; // starting from 0 to 2pi

         // vertex position (x, y, z)
         x = xy * cosf(sectorAngle); // r * cos(u) * cos(v)
         y = xy * sinf(sectorAngle); // r * cos(u) * sin(v)
         vertices.push_back(x);
         vertices.push_back(y);
         vertices.push_back(z);

         nx = x * lengthInv;
         ny = y * lengthInv;
         nz = z * lengthInv;

         if (shade) {
            glm::vec3 normal = glm::vec3(nx, ny, nz);
            glm::vec3 lightDirection =
                glm::normalize(glm::vec3(0.3f, -1.f, 0.5f));
            float shadow = glm::max(glm::dot(normal, lightDirection), 0.f);

            colors.push_back(glm::max(0.f, color.x - shadow));
            colors.push_back(glm::max(0.f, color.y - shadow));
            colors.push_back(glm::max(0.f, color.z - shadow));
         }

         colors.push_back(color.x);
         colors.push_back(color.y);
         colors.push_back(color.z);

         // normalized vertex normal (nx, ny, nz)

         // normals.push_back(nx);
         // normals.push_back(ny);
         // normals.push_back(nz);
      }
   }
   std::vector<unsigned int> indices;
   int k1, k2;
   for (int i = 0; i < stackCount; ++i) {
      k1 = i * (sectorCount + 1); // beginning of current stack
      k2 = k1 + sectorCount + 1;  // beginning of next stack

      for (int j = 0; j < sectorCount; ++j, ++k1, ++k2) {
         // 2 triangles per sector excluding first and last stacks
         // k1 => k2 => k1+1
         if (i != 0) {
            indices.push_back(k1);
            indices.push_back(k2);
            indices.push_back(k1 + 1);
         }

         // k1+1 => k2 => k2+1
         if (i != (stackCount - 1)) {
            indices.push_back(k1 + 1);
            indices.push_back(k2);
            indices.push_back(k2 + 1);
         }
      }
   }
   SetData(indices, vertices, colors);
}

void Mesh::SetPrimitiveCube() {
   // create a cube to represent the playr
#define bottom_front_left 0
#define bottom_back_left 1
#define top_front_left 2
#define top_back_left 3
#define bottom_front_right 4
#define bottom_back_right 5
#define top_front_right 6
#define top_back_right 7

   std::vector<unsigned int> indices{
       // proking vertex takes information from last vertice of tri
       // left face
       bottom_front_left, bottom_back_left, top_front_left, bottom_back_left,
       top_back_left, top_front_left,
       // front face
       top_front_left, top_front_right, bottom_front_left, top_front_right,
       bottom_front_right, bottom_front_left,
       // right face
       bottom_back_right, bottom_front_right, top_back_right,
       bottom_front_right, top_front_right, top_back_right,
       // back face
       bottom_back_right, top_back_right, bottom_back_left, top_back_right,
       top_back_left, bottom_back_left,
       // bottom face
       bottom_back_left, bottom_front_left, bottom_front_right,
       bottom_back_right, bottom_back_left, bottom_front_right,
       // top face
       top_front_left, top_back_left, top_front_right, top_back_left,
       top_back_right, top_front_right};

   std::vector<float> vertices{
       -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 1.0f,  -1.0f, 1.0f,
       -1.0f, -1.0f, 1.0f,  1.0f,  1.0f,  -1.0f, -1.0f, 1.0f,
       -1.0f, 1.0f,  1.0f,  1.0f,  -1.0f, 1.0f,  1.0f,  1.0f,
   };
   // std::vector<float> normals{
   //     0.0f,  0.0f,  1.0f,  // front
   //     0.0f,  0.0f,  -1.0f, // back
   //     -1.0f, 0.0f,  0.0f,  // left
   //     -1.0f, 0.0f,  0.0f,  //
   //     0.0f,  -1.0f, 0.0f,  // bottom
   //     1.0f,  0.0f,  0.0f,  //
   //     0.0f,  1.0f,  0.0f,  // top
   //     1.0f,  0.0f,  0.0f,  // right
   // };
   float shade = 0.3f;
   std::vector<float> colors{shade, shade, shade, shade, shade, shade,
                             shade, shade, shade, shade, shade, shade,
                             shade, shade, shade, shade, shade, shade,
                             shade, shade, shade, shade, shade, shade};
   SetData(indices, vertices, colors);
}

void Mesh::resize() {
   float minX, minY, minZ;
   float maxX, maxY, maxZ;
   float scaleX, scaleY, scaleZ;
   float shiftX, shiftY, shiftZ;
   float epsilon = 0.001f;

   minX = minY = minZ = 1.1754E+38F;
   maxX = maxY = maxZ = -1.1754E+38F;

   // Go through all vertices to determine min and max of each dimension
   for (size_t v = 0; v < vertices.size() / 3; v++) {
      if (vertices[3 * v + 0] < minX)
         minX = vertices[3 * v + 0];
      if (vertices[3 * v + 0] > maxX)
         maxX = vertices[3 * v + 0];

      if (vertices[3 * v + 1] < minY)
         minY = vertices[3 * v + 1];
      if (vertices[3 * v + 1] > maxY)
         maxY = vertices[3 * v + 1];

      if (vertices[3 * v + 2] < minZ)
         minZ = vertices[3 * v + 2];
      if (vertices[3 * v + 2] > maxZ)
         maxZ = vertices[3 * v + 2];
   }

   // From min and max compute necessary scale and shift for each dimension
   float maxExtent, xExtent, yExtent, zExtent;
   xExtent = maxX - minX;
   yExtent = maxY - minY;
   zExtent = maxZ - minZ;
   if (xExtent >= yExtent && xExtent >= zExtent) {
      maxExtent = xExtent;
   }
   if (yExtent >= xExtent && yExtent >= zExtent) {
      maxExtent = yExtent;
   }
   if (zExtent >= xExtent && zExtent >= yExtent) {
      maxExtent = zExtent;
   }
   scaleX = 2.0f / maxExtent;
   shiftX = minX + (xExtent / 2.0f);
   scaleY = 2.0f / maxExtent;
   shiftY = minY + (yExtent / 2.0f);
   scaleZ = 2.0f / maxExtent;
   shiftZ = minZ + (zExtent / 2.0f);

   // Go through all verticies shift and scale them
   for (size_t v = 0; v < vertices.size() / 3; v++) {
      vertices[3 * v + 0] = (vertices[3 * v + 0] - shiftX) * scaleX;
      assert(vertices[3 * v + 0] >= -1.0f - epsilon);
      assert(vertices[3 * v + 0] <= 1.0f + epsilon);
      vertices[3 * v + 1] = (vertices[3 * v + 1] - shiftY) * scaleY;
      assert(vertices[3 * v + 1] >= -1.0f - epsilon);
      assert(vertices[3 * v + 1] <= 1.0f + epsilon);
      vertices[3 * v + 2] = (vertices[3 * v + 2] - shiftZ) * scaleZ;
      assert(vertices[3 * v + 2] >= -1.0f - epsilon);
      assert(vertices[3 * v + 2] <= 1.0f + epsilon);
   }
}

void Mesh::init() {
   // Initialize the vertex array object
   glGenVertexArrays(1, &vaoID);
   glBindVertexArray(vaoID);

   // Send the position array to the GPU
   glGenBuffers(1, &verticesID);
   glBindBuffer(GL_ARRAY_BUFFER, verticesID);
   glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float),
                vertices.data(), GL_STATIC_DRAW);

   // Send the normal array to the GPU
   // if (normals.empty()) {
   //    normalsID = 0;
   // } else {
   //    glGenBuffers(1, &normalsID);
   //    glBindBuffer(GL_ARRAY_BUFFER, normalsID);
   //    glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(float),
   //                 normals.data(), GL_STATIC_DRAW);
   // }

   // Send the texture array to the GPU
   // if (textures.empty()) {
   //    texturesID = 0;
   // } else {
   //    glGenBuffers(1, &texturesID);
   //    glBindBuffer(GL_ARRAY_BUFFER, texturesID);
   //    glBufferData(GL_ARRAY_BUFFER, textures.size() * sizeof(float),
   //                 textures.data(), GL_STATIC_DRAW);
   // }

   // Send the texture array to the GPU
   if (colors.empty()) {
      colorsID = 0;
   } else {
      glGenBuffers(1, &colorsID);
      glBindBuffer(GL_ARRAY_BUFFER, colorsID);
      glBufferData(GL_ARRAY_BUFFER, colors.size() * sizeof(float),
                   colors.data(), GL_STATIC_DRAW);
   }

   // Send the light array to the GPU
   // if (lights.empty()) {
   //    lightsID = 0;
   // } else {
   //    glGenBuffers(1, &lightsID);
   //    glBindBuffer(GL_ARRAY_BUFFER, lightsID);
   //    glBufferData(GL_ARRAY_BUFFER, lights.size() * sizeof(float),
   //                 lights.data(), GL_STATIC_DRAW);
   // }

   // Send the element array to the GPU
   glGenBuffers(1, &indicesID);
   glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indicesID);
   glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int),
                indices.data(), GL_STATIC_DRAW);

   // Unbind the arrays
   glBindBuffer(GL_ARRAY_BUFFER, 0);
   glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

   assert(glGetError() == GL_NO_ERROR);
}

void Mesh::Free() {
  glDeleteBuffers(1, &indicesID);
  glDeleteBuffers(1, &verticesID);
  glDeleteBuffers(1, &colorsID);
  glDeleteVertexArrays(1, &vaoID);
  indices.clear();
  vertices.clear();
  colors.clear();
  indices.shrink_to_fit();
  vertices.shrink_to_fit();
  colors.shrink_to_fit();
}

void Mesh::draw(const Program *prog) const {
   int h_pos, h_col;
   h_pos = h_col = -1;

   glBindVertexArray(vaoID);
   // Bind position buffer
   h_pos = prog->getAttribute("vertPos");
   GLSL::enableVertexAttribArray(h_pos);
   glBindBuffer(GL_ARRAY_BUFFER, verticesID);
   glVertexAttribPointer(h_pos, 3, GL_FLOAT, GL_FALSE, 0, (const void *)0);

   // Bind color buffer
   h_col = prog->getAttribute("color");
   if (h_col != -1 && colorsID != 0) {
      GLSL::enableVertexAttribArray(h_col);
      glBindBuffer(GL_ARRAY_BUFFER, colorsID);
      glVertexAttribPointer(h_col, 3, GL_FLOAT, GL_FALSE, 0, (const void *)0);
   }

   // Bind element buffer
   glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indicesID);

   // // Draw
   glDrawElements(GL_TRIANGLES, (int)indices.size(), GL_UNSIGNED_INT,
                  (const void *)0);

   // Disable and unbind
   // if (h_tex != -1) {
   //    GLSL::disableVertexAttribArray(h_tex);
   // }
   if (h_col != -1) {
      GLSL::disableVertexAttribArray(h_col);
   }
   GLSL::disableVertexAttribArray(h_pos);
   glBindBuffer(GL_ARRAY_BUFFER, 0);
   glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

// void Mesh::loadMesh(const string &meshName) {
//    // Load geometry
//    // Some obj files contain material information.
//    // We'll ignore them for this assignment.
//    vector<tinyobj::shape_t> shapes;
//    vector<tinyobj::material_t> objMaterials;
//    string errStr;
//    bool rc = tinyobj::LoadObj(shapes, objMaterials, errStr,
//    meshName.c_str());

//    if (!rc) {
//       cerr << errStr << endl;
//    } else if (shapes.size()) {
//       vertices = shapes[0].mesh.positions;
//       normals = shapes[0].mesh.normals;
//       textures = shapes[0].mesh.texcoords;
//       indices = shapes[0].mesh.indices;
//    }
// }
