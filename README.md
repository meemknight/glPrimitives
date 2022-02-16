# glPrimitives

![demo](https://github.com/meemknight/photos/blob/master/glPrimitives.png)

---

A simple library for drawing simple 3d shapes

---

- Functions

```cpp

//inits the renderer
void init(int w, int h);

//updates window size
void updateWindowMetrics(int w, int h);

//renderes the primitives
void flush(Camera &camera);

//draws a wired triangle. 
void drawTriangleMesh(glm::vec3 a, glm::vec3 b, glm::vec3 c, glm::vec3 color);

//draws a solid triangle
void drawTriangle(glm::vec3 a, glm::vec3 b, glm::vec3 c, glm::vec4 color);

//draws a solid triangle and a border around it
void drawTriangleWithBorder(glm::vec3 a, glm::vec3 b, glm::vec3 c, glm::vec4 color);

//draws a line
void drawLine(glm::vec3 a, glm::vec3 b, glm::vec3 color);

//draws a point
void drawPoint(glm::vec3 a, glm::vec3 color);

```
