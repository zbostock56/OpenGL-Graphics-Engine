#include <main.h>

void keyboard_input(GLFWwindow *window);
void mouse_input(GLFWwindow *window, double xpos, double ypos);

float delta_time = 0.0;
float last_frame = 0.0;

vec3 camera_pos = { 0.0, 0.0, 3.0 };
vec3 camera_front = { 0.0, 0.0, -1.0 };
vec3 camera_up = { 0.0, 1.0, 0.0 };
vec3 center = { 0.0, 0.0, 0.0 };

float lastX = 400;
float lastY = 300;

float pitch = 0;
float yaw = 0;

int firstMouse = 1;

int main() {
  GLFWwindow *window;

  if (!glfwInit()) {
    return -1;
  }

  window = glfwCreateWindow(640, 480, "Jack", NULL, NULL);
  if (window == NULL) {
    printf("Failed to create GLFW window\n");
    glfwTerminate();
    return -1;
  }
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

  glfwSetInputMode(window, GLFW_CURSOR,GLFW_CURSOR_DISABLED);
  glfwSetCursorPosCallback(window, mouse_input);

  glfwMakeContextCurrent(window);

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    printf("Failed to initialize GLAD\n");
    glfwTerminate();
    return -1;
  }

  glViewport(0, 0, 640, 480);

  unsigned int shader = init_shader_prog(
      //"C:/Users/Jack/Documents/C/OpenGL-Graphics-Engine/src/shaders/test/shader.vs",
      "C:/Users/jackm/Documents/C/OpenGL-Graphics-Engine/src/shaders/test/shader.vs",
      //"C:/Users/jackm/Documents/C/OpenGL-Graphics-Engine/src/shaders/phong/shader.vs",
      NULL,
      //"C:/Users/Jack/Documents/C/OpenGL-Graphics-Engine/src/shaders/test/shader.fs"
      "C:/Users/jackm/Documents/C/OpenGL-Graphics-Engine/src/shaders/test/shader.fs"
      //"C:/Users/jackm/Documents/C/OpenGL-Graphics-Engine/src/shaders/phong/shader.fs"
      );
  if (shader == -1) {
    printf("Error loading shaders\n");
    glfwTerminate();
    return -1;
  }

  unsigned int b_shader = init_shader_prog(
      //"C:/Users/Jack/Documents/C/OpenGL-Graphics-Engine/src/shaders/bone/shader.vs",
      "C:/Users/jackm/Documents/C/OpenGL-Graphics-Engine/src/shaders/bone/shader.vs",
      NULL,
      //"C:/Users/Jack/Documents/C/OpenGL-Graphics-Engine/src/shaders/bone/shader.fs"
      "C:/Users/jackm/Documents/C/OpenGL-Graphics-Engine/src/shaders/bone/shader.fs"
      );
  if (b_shader == -1) {
    printf("Error loading bone shaders\n");
    glfwTerminate();
    return -1;
  }

  MODEL *cube = load_model(
      //"C:/Users/Jack/Documents/C/OpenGL-Graphics-Engine/resources/cube/cube.obj"
      "C:/Users/jackm/Documents/C/OpenGL-Graphics-Engine/resources/cube/cube.obj"
      );
  if (cube == NULL) {
    printf("Unable to load model\n");
    glfwTerminate();
    return -1;
  }

  MODEL *cross = load_model(
      //"C:/Users/Jack/Documents/C/OpenGL-Graphics-Engine/resources/cross/cross.obj"
      "C:/Users/jackm/Documents/C/OpenGL-Graphics-Engine/resources/cross/cross.obj"
      );
  if (cross == NULL) {
    printf("Unable to load model\n");
    glfwTerminate();
    return -1;
  }

  MODEL *dude = load_model(
      //"C:/Users/Jack/Documents/C/OpenGL-Graphics-Engine/resources/low_poly_new/low_poly_new.obj"
      "C:/Users/jackm/Documents/C/OpenGL-Graphics-Engine/resources/low_poly_new/low_poly_new.obj"
      );
  if (dude == NULL) {
    printf("Unable to load model\n");
    glfwTerminate();
    return -1;
  }

  MODEL *test = load_model(
      //"C:/Users/Jack/Documents/C/OpenGL-Graphics-Engine/resources/test/test.obj"
      "C:/Users/jackm/Documents/C/OpenGL-Graphics-Engine/resources/test/test.obj"
      );
  if (test == NULL) {
    printf("Unable to load model\n");
    glfwTerminate();
    return -1;
  }




// NEW ANIM FUNCTIONALITY
  /*for (int i = 0; i < test->num_animations; i++) {
    printf("Animation %d: %lld chains, %lld frames\n", i,
           test->animations[i].num_chains, test->animations[i].duration);
    for (int j = 0; j < test->animations[i].num_chains; j++) {
      printf("  Chain of type %d with %lld frames on bone: %d\n",
             test->animations[i].keyframe_chains[j].type,
             test->animations[i].keyframe_chains[j].num_frames,
             test->animations[i].keyframe_chains[j].b_id
             );
      printf("  ");
      for (int k = 0; k < test->animations[i].duration; k++) {
        printf("%d ", test->animations[i].keyframe_chains[j].sled[k]);
      }
      printf("\n  ");
      for (int k = 0; k < test->animations[i].keyframe_chains[j].num_frames; k++) {
        printf("%d:(%f %f %f %f) ",
               test->animations[i].keyframe_chains[j].chain[k].frame,
               test->animations[i].keyframe_chains[j].chain[k].offset[0],
               test->animations[i].keyframe_chains[j].chain[k].offset[1],
               test->animations[i].keyframe_chains[j].chain[k].offset[2],
               test->animations[i].keyframe_chains[j].chain[k].offset[3]);
      }
      printf("\n");
    }
  }
  fflush(stdout);*/
  int cur_frame = 0;
// END NEW




  mat4 projection = GLM_MAT4_IDENTITY_INIT;
  glm_perspective(glm_rad(45.0f), 800.0f / 600.0f, 0.1f, 100.0f, projection);
  glUseProgram(shader);
  glUniformMatrix4fv(glGetUniformLocation(shader, "projection"), 1,
                     GL_FALSE, (float *)projection);

  glUseProgram(b_shader);
  glUniformMatrix4fv(glGetUniformLocation(b_shader, "projection"), 1,
                     GL_FALSE, (float *)projection);

  glEnable(GL_DEPTH_TEST);

  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

  float point[] = { 0.0, 0.0, 0.0 };
  unsigned int pt_VAO;
  glGenVertexArrays(1, &pt_VAO);
  glBindVertexArray(pt_VAO);

  unsigned int pt_VBO;
  glGenBuffers(1, &pt_VBO);
  glBindBuffer(GL_ARRAY_BUFFER, pt_VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3, point, GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float),
                        (void *) 0);
  glEnableVertexAttribArray(0);
  glBindVertexArray(0);

  float until_next = 0.0;
  while (!glfwWindowShouldClose(window)) {
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    float current_time = glfwGetTime();
    delta_time = current_time - last_frame;
    last_frame = current_time;

    until_next += delta_time;

    keyboard_input(window);




// NEW ANIM FUNCTIONALITY
    animate(test, 1, cur_frame);
    if (until_next >= 0.125) {
      cur_frame++;
      until_next = 0.0;
    }
// END NEW




    // Render
    glUseProgram(shader);

    mat4 view = GLM_MAT4_IDENTITY_INIT;
    glm_vec3_add(camera_front, camera_pos, center);
    glm_lookat(camera_pos, center, camera_up, view);

    mat4 model = GLM_MAT4_IDENTITY_INIT;
    vec3 translation = { 0.25, 0.25, 0.25 };
    glm_scale(model, translation);
    //glm_translate(model, translation);

    for (int i = 0; i < test->num_bones; i++) {
      char var_name[50];
      sprintf(var_name, "bones[%d].coords", i);
      glUniform3f(glGetUniformLocation(shader, var_name),
                  test->bones[i].coords[0], test->bones[i].coords[1],
                  test->bones[i].coords[2]);
      sprintf(var_name, "bones[%d].parent", i);
      glUniform1i(glGetUniformLocation(shader, var_name),
                   test->bones[i].parent);

      sprintf(var_name, "bone_mats[%d]", i);
      glUniformMatrix4fv(glGetUniformLocation(shader, var_name),
                         3, GL_FALSE,
                         (float *) test->bone_mats[i]);
      /*printf("%d\n", i);
      for (int j = 0; j < 4; j++) {
        printf("|%f %f %f %f|%f %f %f %f|%f %f %f %f|\n",
               bone_transformations[i][0][j][0],
               bone_transformations[i][0][j][1],
               bone_transformations[i][0][j][2],
               bone_transformations[i][0][j][3],
               bone_transformations[i][1][j][0],
               bone_transformations[i][1][j][1],
               bone_transformations[i][1][j][2],
               bone_transformations[i][1][j][3],
               bone_transformations[i][2][j][0],
               bone_transformations[i][2][j][1],
               bone_transformations[i][2][j][2],
               bone_transformations[i][2][j][3]);
      }
      printf("\n");
      fflush(stdout);*/
    }


    glUniformMatrix4fv(glGetUniformLocation(shader, "model"), 1,
                       GL_FALSE, (float *) model);

    glUniformMatrix4fv(glGetUniformLocation(shader, "view"), 1,
                       GL_FALSE, (float *) view);

    draw_model(shader, test);
    //draw_model(shader, cube);
    //draw_model(shader, cross);
    //draw_model(shader, dude);

    glUseProgram(b_shader);

    for (int i = 0; i < test->num_bones; i++) {
      char var_name[50];
      sprintf(var_name, "bones[%d].coords", i);
      glUniform3f(glGetUniformLocation(b_shader, var_name),
                  test->bones[i].coords[0], test->bones[i].coords[1],
                  test->bones[i].coords[2]);
      sprintf(var_name, "bones[%d].parent", i);
      glUniform1i(glGetUniformLocation(b_shader, var_name),
                   test->bones[i].parent);

      sprintf(var_name, "bone_mats[%d]", i);
      glUniformMatrix4fv(glGetUniformLocation(b_shader, var_name),
                         3, GL_FALSE,
                         (float *) test->bone_mats[i]);
    }

    glUniform4f(glGetUniformLocation(b_shader, "col"), 1.0, 0.0, 0.0, 1.0);
    glUniformMatrix4fv(glGetUniformLocation(b_shader, "model"), 1,
                       GL_FALSE, (float *) model);
    glUniformMatrix4fv(glGetUniformLocation(b_shader, "view"), 1,
                       GL_FALSE, (float *) view);
    glPointSize(10.0);

    /*glBindVertexArray(pt_VAO);
    glDrawArrays(GL_POINTS, 0, 1);
    glBindVertexArray(0);*/

    glUniform4f(glGetUniformLocation(b_shader, "col"), 0.0, 0.0, 1.0, 1.0);
    draw_bones(test);

    // Swap Buffers and Poll Events
    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  free_model(cube);
  free_model(test);
  free_model(cross);
  free_model(dude);

  glfwTerminate();

  return 0;
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
  glViewport(0, 0, width, height);
}

void keyboard_input(GLFWwindow *window) {
  float cam_speed = 2.5 * delta_time;
  vec3 movement = { 0.0, 0.0, 0.0 };
  if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
    glm_vec3_scale(camera_front, cam_speed, movement);
    glm_vec3_add(camera_pos, movement, camera_pos);
  }
  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
    glm_vec3_scale(camera_front, cam_speed, movement);
    glm_vec3_sub(camera_pos, movement, camera_pos);
  }
  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
    glm_vec3_cross(camera_front, camera_up, movement);
    glm_vec3_normalize(movement);
    glm_vec3_scale(movement, cam_speed, movement);
    glm_vec3_sub(camera_pos, movement, camera_pos);
  }
  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
    glm_vec3_cross(camera_front, camera_up, movement);
    glm_vec3_normalize(movement);
    glm_vec3_scale(movement, cam_speed, movement);
    glm_vec3_add(camera_pos, movement, camera_pos);
  }
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, 1);
  }
}

void mouse_input(GLFWwindow *window, double xpos, double ypos) {
  if (firstMouse) {
    lastX = xpos;
    lastY = yaw;
    firstMouse = 0;
  }

  float xOffset = xpos -lastX;
  float yOffset = lastY - ypos;

  lastX = xpos;
  lastY = ypos;

  const float sensitivity = 0.1f;
  xOffset *= sensitivity;
  yOffset *= sensitivity;

  yaw += xOffset;
  pitch += yOffset;

  if (pitch > 89.0f) {
    pitch = 89.0f;
  } else if (pitch < -89.0f) {
    pitch = -89.0f;
  }

  camera_front[0] = cos(glm_rad(yaw)) * cos(glm_rad(pitch));
  camera_front[1] = sin(glm_rad(pitch));
  camera_front[2] = sin(glm_rad(yaw)) * cos(glm_rad(pitch));
  glm_vec3_normalize(camera_front);
}
