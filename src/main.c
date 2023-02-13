#include <main.h>

#define LINUX (1)
#define LAPTOP (0)
#define PC (0)

#if LINUX == 1
#define DIR "/home/jbs/Documents/C/OpenGL-Graphics-Engine"
#elif LAPTOP == 1
#define DIR "C:/Users/jackm/Documents/C/OpenGL-Graphics-Engine"
#elif PC == 1
#define DIR "C:/Users/Jack/Documents/C/OpenGL-Graphics-Engine"
#else
#define DIR ""
#endif


vec3 up = { 0.0, 1.0, 0.0 };

float delta_time = 0.0;
float last_frame = 0.0;

vec3 camera_offset = { 0.0, 0.0, -5.0 };
vec3 camera_front = { 0.0, 0.0, -1.0 };
vec3 camera_pos = { 0.0, 0.0, 0.0 };
vec3 camera_model_pos = { 0.0, 0.0, 0.0 };
float camera_model_rot = 0.0;

vec3 movement = { 0.0, 0.0, 0.0 };

float lastX = 400;
float lastY = 300;

float pitch = 0;
float yaw = 0;

int firstMouse = 1;

int cur_frame = 0;
float last_push = 0.0;

int toggled = 1;
int draw = 0;

int main() {
  GLFWwindow *window;

  if (!glfwInit()) {
    return -1;
  }
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  window = glfwCreateWindow(640, 480, "Jack", NULL, NULL);
  if (window == NULL) {
    printf("Failed to create GLFW window\n");
    glfwTerminate();
    return -1;
  }

  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

  glfwSetInputMode(window, GLFW_CURSOR,GLFW_CURSOR_DISABLED);
  glfwSetCursorPosCallback(window, mouse_input);
  glfwSetScrollCallback(window, scroll_callback);

  glfwMakeContextCurrent(window);

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    printf("Failed to initialize GLAD\n");
    glfwTerminate();
    return -1;
  }

  glViewport(0, 0, 640, 480);

  unsigned int shader = init_shader_prog(
      DIR"/src/shaders/cell_shader/shader.vs",
      NULL,
      DIR"/src/shaders/cell_shader/shader.fs"
      );
  if (shader == -1) {
    printf("Error loading shaders\n");
    glfwTerminate();
    return -1;
  }

  unsigned int u_shader = init_shader_prog(
      DIR"/src/shaders/unanimated/shader.vs",
      NULL,
      DIR"/src/shaders/cell_shader/shader.fs"
      );
  if (u_shader == -1) {
    printf("Error loading shaders\n");
    glfwTerminate();
    return -1;
  }

  unsigned int bone_shader = init_shader_prog(
      DIR"/src/shaders/bone/shader.vs",
      NULL,
      DIR"/src/shaders/basic/shader.fs"
      );
  if (bone_shader == -1) {
    printf("Error loading bone shaders\n");
    glfwTerminate();
    return -1;
  }

  unsigned int basic_shader = init_shader_prog(
      DIR"/src/shaders/basic/shader.vs",
      NULL,
      DIR"/src/shaders/basic/shader.fs"
      );
  if (basic_shader == -1) {
    printf("Error loading basic shaders\n");
    glfwTerminate();
    return -1;
  }

  unsigned int test_shader = init_shader_prog(
      DIR"/src/shaders/unanimated/shader.vs",
      NULL,
      DIR"/src/shaders/test/shader.fs"
      );
  if (test_shader == -1) {
    printf("Error loading test shaders\n");
    glfwTerminate();
    return -1;
  }

  MODEL *cube = load_model(
      DIR"/resources/cube/cube.obj"
      );
  if (cube == NULL) {
    printf("Unable to load cube model\n");
    glfwTerminate();
    return -1;
  }

  MODEL *dude = load_model(
      DIR"/resources/low_poly_new/low_poly_new.obj"
      );
  if (dude == NULL) {
    printf("Unable to load dude model\n");
    glfwTerminate();
    return -1;
  }

  MODEL *test = load_model(
      DIR"/resources/test/test.obj"
      );
  if (test == NULL) {
    printf("Unable to load test model\n");
    glfwTerminate();
    return -1;
  }

  MODEL *floor = load_model(
      DIR"/resources/floor/floor.obj"
      );
  if (floor == NULL) {
    printf("Unable to load floor model\n");
    glfwTerminate();
    return -1;
  }

  MODEL *platform = load_model(
      DIR"/resources/platform/platform.obj"
      );
  if (platform == NULL) {
    printf("Unable to load platform model\n");
    glfwTerminate();
    return -1;
  }

  MODEL *sphere = load_model(
      DIR"/resources/sphere/sphere.obj"
      );
  if (sphere == NULL) {
    printf("Unable to load spehere model\n");
    glfwTerminate();
    return -1;
  }

  MODEL *vector = load_model(
      DIR"/resources/vector/vector.obj"
      );
  if (vector == NULL) {
    printf("Unable to load vector model\n");
    glfwTerminate();
    return -1;
  }

  /*OCT_TREE *tree = init_tree();
  if (tree == NULL) {
    printf("Unable to load oct-tree\n");
    glfwTerminate();
    return -1;
  }*/

  ENTITY *player = init_entity(dude);
  if (player == NULL) {
    printf("Unable to load player\n");
    glfwTerminate();
    return -1;
  }

  ENTITY *obstacle = init_entity(platform);
  if (obstacle == NULL) {
    printf("Unable to load obstacle\n");
    glfwTerminate();
    return -1;
  }
  vec3 ob_pos = { 3.0, 0.0, -3.0 };
  glm_mat4_identity(obstacle->model_mat);
  glm_translate(obstacle->model_mat, ob_pos);



  vec3 cube_pos = { 3.0, 2.0, 3.0 };
  vec3 cube_col = { 1.0, 1.0, 1.0 };
  cube->num_colliders = 1;
  cube->colliders = malloc(sizeof(COLLIDER));
  cube->collider_bone_links = malloc(sizeof(int));
  cube->collider_bone_links[0] = -1;
  ENTITY *box_entity = init_entity(cube);
  if (box_entity == NULL) {
    printf("Unable to load box entity\n");
    glfwTerminate();
    return -1;
  }
  vec3 verts[8] = {
    {  1.0,  1.0,  1.0 },
    {  1.0,  1.0, -1.0 },
    { -1.0,  1.0,  1.0 },
    { -1.0,  1.0, -1.0 },
    {  1.0, -1.0,  1.0 },
    {  1.0, -1.0, -1.0 },
    { -1.0, -1.0,  1.0 },
    { -1.0, -1.0, -1.0 }
  };
  COLLIDER box;
  box.type = POLY;
  box.data.num_used = 8;
  for (int i = 0; i < 8; i++) {
    glm_vec3_copy(verts[i], box.data.verts[i]);
  }
  cube->colliders[0] = box;
  glm_mat4_identity(box_entity->model_mat);
  glm_translate(box_entity->model_mat, cube_pos);
  //oct_tree_insert(tree, box_entity, 0);

  vec3 s_pos = { -3.0, 2.0, -3.0 };
  vec3 s_col = { 1.0, 1.0, 1.0 };
  sphere->num_colliders = 1;
  sphere->colliders = malloc(sizeof(COLLIDER));
  sphere->collider_bone_links = malloc(sizeof(int));
  sphere->collider_bone_links[0] = -1;
  ENTITY *sphere_entity = init_entity(sphere);
  if (sphere_entity == NULL) {
    printf("Unable to load sphere entity\n");
    glfwTerminate();
    return -1;
  }
  COLLIDER ball;
  ball.type = SPHERE;
  ball.data.center[0] = 0.0;
  ball.data.center[1] = 0.0;
  ball.data.center[2] = 0.0;
  ball.data.radius = 1.0;
  sphere->colliders[0] = ball;
  glm_mat4_identity(sphere_entity->model_mat);
  glm_translate(sphere_entity->model_mat, s_pos);
  // oct_tree_insert(tree, sphere_entity, 0);




  glm_translate(player->model_mat, camera_model_pos);
  glm_rotate_y(player->model_mat, camera_model_rot, player->model_mat);
  /*int status = oct_tree_insert(tree, player, 15);
  if (status != 0) {
    printf("Failed to insert dude1\n");
  }*/

  mat4 projection = GLM_MAT4_IDENTITY_INIT;
  mat4 model = GLM_MAT4_IDENTITY_INIT;
  mat4 view = GLM_MAT4_IDENTITY_INIT;

  glm_perspective(glm_rad(45.0f), 800.0f / 600.0f, 0.1f, 100.0f, projection);
  glUseProgram(shader);
  glUniformMatrix4fv(glGetUniformLocation(shader, "projection"), 1,
                     GL_FALSE, (float *)projection);

  glUseProgram(u_shader);
  glUniformMatrix4fv(glGetUniformLocation(u_shader, "projection"), 1,
                     GL_FALSE, (float *)projection);

  glUseProgram(bone_shader);
  glUniformMatrix4fv(glGetUniformLocation(bone_shader, "projection"), 1,
                     GL_FALSE, (float *)projection);

  glUseProgram(basic_shader);
  glUniformMatrix4fv(glGetUniformLocation(basic_shader, "projection"), 1,
                     GL_FALSE, (float *)projection);

  glUseProgram(test_shader);
  glUniformMatrix4fv(glGetUniformLocation(test_shader, "projection"), 1,
                     GL_FALSE, (float *)projection);

  glEnable(GL_DEPTH_TEST);

  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

  /* Origin VAO setup */

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


  // SIMULATION SET UP

  int status = init_simulation();
  if (status != 0) {
    glfwTerminate();
  }

  // Make player driving
  player->type |= 2;
  status = insert_entity(player);
  if (status != 0) {
    glfwTerminate();
  }

  // Make obstacle immovable
  obstacle->type |= 4;
  status = insert_entity(obstacle);
  if (status != 0) {
    glfwTerminate();
  }

  while (!glfwWindowShouldClose(window)) {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    float current_time = glfwGetTime();
    delta_time = current_time - last_frame;
    last_frame = current_time;

    until_next += delta_time;

    keyboard_input(window);

    /* Animation */

    animate(player, 0, cur_frame);
    cube_pos[0] = 3.0;
    cube_pos[1] = 3.0 + sin(glfwGetTime());
    cube_pos[2] = 3.0;
    glm_mat4_identity(box_entity->model_mat);
    glm_translate(box_entity->model_mat, cube_pos);
    s_pos[0] = -3.0;
    s_pos[1] = 2.0 + sin(glfwGetTime());
    s_pos[2] = -3.0;
    glm_mat4_identity(sphere_entity->model_mat);
    glm_translate(sphere_entity->model_mat, s_pos);

    /* Physics */

    glm_vec3_copy(movement, player->velocity);
    vec3 displacement = { 0.0, 0.0, 0.0 };
    glm_vec3(player->model_mat[3], displacement);
    status = simulate_frame();
    if (status != 0) {
      break;
    }
    vec3 update = { 0.0, 0.0, 0.0 };
    glm_vec3(player->model_mat[3], update);
    float temp = update[0];
    update[0] = update[2];
    update[2] = temp;
    glm_vec3_sub(player->model_mat[3], displacement, displacement);
    glm_vec3_add(displacement, camera_model_pos, camera_model_pos);

    /*COLLIDER a;
    a.type = POLY;
    a.data.num_used = player->model->colliders[0].data.num_used;
    for (int i = 0; i < 8; i++) {
      glm_mat4_mulv3(player->model_mat,
                     player->model->colliders[0].data.verts[i], 1.0,
                     a.data.verts[i]);
    }
    //glm_mat4_mulv3(player->model_mat, player->model->colliders[15].data.center,
    //               1.0, a.data.center);
    
    COLLIDER b;
    b.type = POLY;
    b.data.num_used = 8;
    for (int i = 0; i < 8; i++) {
      glm_mat4_mulv3(obstacle->model_mat,
                     obstacle->model->colliders[0].data.verts[i], 1.0,
                     b.data.verts[i]);
    }
    COLLIDER c;
    c.type = SPHERE;
    c.data.radius = sphere_entity->model->colliders[0].data.radius;
    glm_mat4_mulv3(sphere_entity->model_mat,
                   sphere_entity->model->colliders[0].data.center, 1.0,
                   c.data.center);

    vec3 simplex[4];
    vec3 collision_dir[2];
    float collision_depth[2];
    versor temp;
    mat4 vector_rot_mats[2];
    if (collision_check(&a, &b, simplex)) {
      status = epa_response(&a, &b, simplex, collision_dir[0],
                            collision_depth);
      if (status != 0) {
        printf("COLLISION ERR BETWEEN PLATFORM AND PLAYER\n");
        break;
      }
      glm_vec3_negate(collision_dir[0]);
      glm_quat_from_vecs(up, collision_dir[0], temp);
      glm_quat_mat4(temp, vector_rot_mats[0]);

      cube_col[0] = 1.0;
      cube_col[1] = 0.0;
      cube_col[2] = 0.0;
    } else {
      glm_vec3_zero(collision_dir[0]);
      collision_depth[0] = 0;

      cube_col[0] = 1.0;
      cube_col[1] = 1.0;
      cube_col[2] = 1.0;
    }
    if (collision_check(&a, &c, simplex)) {
      status = epa_response(&a, &c, simplex, collision_dir[1],
                            collision_depth + 1);
      if (status != 0) {
        printf("COLLISION ERR BETWEEN SPHERE AND PLAYER\n");
        break;
      }
      glm_vec3_negate(collision_dir[1]);
      glm_quat_from_vecs(up, collision_dir[1], temp);
      glm_quat_mat4(temp, vector_rot_mats[1]);

      s_col[0] = 1.0;
      s_col[1] = 0.0;
      s_col[2] = 0.0;
    } else {
      glm_vec3_zero(collision_dir[1]);
      collision_depth[1] = 0;

      s_col[0] = 1.0;
      s_col[1] = 1.0;
      s_col[2] = 1.0;
    }*/

    /* Collision */

    //oct_tree_delete(tree, player->tree_offsets[15]);
    //oct_tree_insert(tree, player, 15);
    //oct_tree_delete(tree, box_entity->tree_offsets[0]);
    //oct_tree_insert(tree, box_entity, 0);
    //oct_tree_delete(tree, sphere_entity->tree_offsets[0]);
    //oct_tree_insert(tree, sphere_entity, 0);

    // Render

    /* Camera */

    vec3 translation = { -camera_model_pos[0], -camera_model_pos[1],
                         -camera_model_pos[2] };

    glm_mat4_identity(view);
    camera_offset[1] = -dude->bones[18].coords[1];
    glm_translate(view, camera_offset);
    glm_rotate_x(view, glm_rad(pitch), view);
    glm_rotate_y(view, glm_rad(yaw), view);
    glm_translate(view, translation);

    mat4 rot_mat = GLM_MAT4_IDENTITY_INIT;
    glm_mat4_inv_fast(view, rot_mat);
    glm_vec3_zero(camera_pos);
    glm_mat4_mulv3(rot_mat, camera_pos, 1.0, camera_pos);

    /* Models */

    glm_mat4_identity(model);

    /* Origin */

    glPointSize(10.0);
    glUseProgram(basic_shader);
    glUniformMatrix4fv(glGetUniformLocation(basic_shader, "model"), 1,
                       GL_FALSE, (float *) model);
    glUniformMatrix4fv(glGetUniformLocation(basic_shader, "view"), 1,
                       GL_FALSE, (float *) view);
    glUniform3f(glGetUniformLocation(basic_shader, "test_col"), 0.0, 1.0, 1.0);
    glBindVertexArray(pt_VAO);
    glDrawArrays(GL_POINTS, 0, 1);
    glBindVertexArray(0);

    /* Oct-Tree */

    /* Skeleton */

    glm_translate(model, camera_model_pos);
    glm_rotate_y(model, camera_model_rot, model);

    glUseProgram(bone_shader);
    glUniform3f(glGetUniformLocation(bone_shader, "test_col"), 0.0, 0.0, 1.0);
    glUniformMatrix4fv(glGetUniformLocation(bone_shader, "view"), 1,
                       GL_FALSE, (float *) view);

    glm_mat4_copy(model, player->model_mat);
    draw_skeleton(bone_shader, player);

    /* Colliders */

    glPointSize(5.0);
    glUseProgram(basic_shader);
    glUniform3f(glGetUniformLocation(basic_shader, "test_col"), 1.0, 0.0, 1.0);
    glBindVertexArray(pt_VAO);
    draw_colliders(basic_shader, player, sphere);
    draw_colliders(basic_shader, obstacle, sphere);

    glUniform3f(glGetUniformLocation(basic_shader, "test_col"), cube_col[0],
                cube_col[1], cube_col[2]);
    draw_colliders(basic_shader, box_entity, sphere);

    glUniform3f(glGetUniformLocation(basic_shader, "test_col"), s_col[0],
                s_col[1], s_col[2]);
    draw_colliders(basic_shader, sphere_entity, sphere);

    /* Skin */

    glUseProgram(shader);
    glUniformMatrix4fv(glGetUniformLocation(shader, "view"), 1,
                       GL_FALSE, (float *) view);
    glUniform3f(glGetUniformLocation(shader, "camera_pos"), camera_pos[0],
                camera_pos[1], camera_pos[2]);
    if (draw) {
      draw_entity(shader, player);
    }

    /* Unanimated models */

    glUseProgram(u_shader);
    glUniformMatrix4fv(glGetUniformLocation(u_shader, "view"), 1,
                       GL_FALSE, (float *) view);
    glUniform3f(glGetUniformLocation(u_shader, "camera_pos"), camera_pos[0],
                camera_pos[1], camera_pos[2]);

    glm_mat4_identity(model);
    glm_vec3_zero(translation);
    translation[0] = 5.0;
    translation[1] = 10.0;
    translation[2] = -15.0;
    glm_translate(model, translation);
    glUniformMatrix4fv(glGetUniformLocation(u_shader, "model"), 1,
                       GL_FALSE, (float *) model);
    draw_model(u_shader, dude);

    /*vec3 scale_factor = { 1.0, 1.0, 1.0 };
    mat4 t_mat = GLM_MAT4_IDENTITY_INIT;
    scale_factor[1] = collision_depth[0];
    glm_translate(t_mat, ob_pos);
    glm_mat4_identity(model);
    glm_mat4_mul(vector_rot_mats[0], model, model);
    glm_mat4_mul(t_mat, model, model);
    glm_scale(model, scale_factor);
    glUniformMatrix4fv(glGetUniformLocation(u_shader, "model"), 1,
                       GL_FALSE, (float *) model);
    draw_model(u_shader, vector);

    scale_factor[1] = collision_depth[1];
    glm_mat4_identity(model);
    glm_mat4_identity(t_mat);
    glm_translate(t_mat, s_pos);
    glm_mat4_mul(vector_rot_mats[1], model, model);
    glm_mat4_mul(t_mat, model, model);
    glm_scale(model, scale_factor);
    glUniformMatrix4fv(glGetUniformLocation(u_shader, "model"), 1,
                       GL_FALSE, (float *) model);
    draw_model(u_shader, vector);*/

    glm_mat4_identity(model);
    glm_vec3_zero(translation);
    translation[0] = 50.0;
    translation[1] = 50.0;
    translation[2] = 50.0;
    glm_scale(model, translation);
    glUniformMatrix4fv(glGetUniformLocation(u_shader, "model"), 1,
                       GL_FALSE, (float *) model);
    draw_model(u_shader, floor);

    /* Tests */

    glUseProgram(test_shader);
    glUniformMatrix4fv(glGetUniformLocation(test_shader, "view"), 1, GL_FALSE,
                       (float *) view);
    glUniform3f(glGetUniformLocation(test_shader, "test_col"), 1.0, 1.0, 0.0);

    //vec3 pos = { 0.0, 0.0, 0.0 };
    //draw_oct_tree(cube, tree, pos, 16.0, test_shader, 0, 1);

    glUniform3f(glGetUniformLocation(test_shader, "test_col"), cube_col[0],
                cube_col[1], cube_col[2]);
    glUniformMatrix4fv(glGetUniformLocation(test_shader, "model"), 1,
                       GL_FALSE, (float *) box_entity->model_mat);
    draw_entity(test_shader, box_entity);

    glm_mat4_identity(model);
    glm_translate(model, ob_pos);
    glUniformMatrix4fv(glGetUniformLocation(test_shader, "model"), 1,
                       GL_FALSE, (float *) obstacle->model_mat);
    draw_entity(test_shader, obstacle);


    // Swap Buffers and Poll Events
    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  end_simulation();
  free_entity(player);
  free_entity(box_entity);
  free_entity(sphere_entity);

  free_model(cube);
  free_model(test);
  free_model(dude);
  free_model(floor);
  free_model(sphere);
  free_model(vector);

  //free_oct_tree(tree);

  glfwTerminate();

  return 0;
}

vec3 quad_translate[8] = {
                       { 1.0, 1.0, 1.0 }, //  X, Y, Z
                       { 1.0, 1.0, -1.0 }, //  X, Y,-Z
                       { 1.0, -1.0, 1.0 }, //  X,-Y, Z
                       { 1.0, -1.0, -1.0 }, //  X,-Y,-Z
                       { -1.0, 1.0, 1.0 }, // -X, Y, Z
                       { -1.0, 1.0, -1.0 }, // -X, Y,-Z
                       { -1.0, -1.0, 1.0 }, // -X,-Y, Z
                       { -1.0, -1.0, -1.0 }  // -X,-Y,-Z
                      };
void draw_oct_tree(MODEL *cube, OCT_TREE *tree, vec3 pos, float scale,
                   unsigned int shader, size_t offset, int depth) {
  mat4 model = GLM_MAT4_IDENTITY_INIT;
  glm_translate(model, pos);
  glm_scale_uni(model, scale);
  glUniformMatrix4fv(glGetUniformLocation(shader, "model"), 1,
                     GL_FALSE, (float *) model);
  draw_model(shader, cube);

  vec3 temp = { 0.0, 0.0, 0.0 };
  if (tree->node_buffer[offset].next_offset != -1 && depth < 5) {
    for (int i = 0; i < 8; i++) {
      glUniform3f(glGetUniformLocation(shader, "test_col"), 1.0, 1.0, 0.0);
      glm_vec3_scale(quad_translate[i], scale / 2.0, temp);
      glm_vec3_add(pos, temp, temp);
      draw_oct_tree(cube, tree, temp, scale / 2.0, shader,
                    tree->node_buffer[offset].next_offset + i, depth + 1);
    }
  }
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
  glViewport(0, 0, width, height);
}

void keyboard_input(GLFWwindow *window) {
  float cam_speed = 2.0 * delta_time;
  glm_vec3_zero(movement);
  if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
    camera_model_rot = glm_rad(-yaw + 180.0);

    glm_vec3_scale(camera_front, cam_speed, movement);
    glm_vec3_add(camera_model_pos, movement, camera_model_pos);
  }
  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
    camera_model_rot = glm_rad(-yaw);

    glm_vec3_scale(camera_front, -cam_speed, movement);
    glm_vec3_add(camera_model_pos, movement, camera_model_pos);
  }
  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
    camera_model_rot = glm_rad(-yaw -90.0);

    glm_vec3_cross(up, camera_front, movement);
    glm_vec3_normalize(movement);
    glm_vec3_scale(movement, cam_speed, movement);
    glm_vec3_add(camera_model_pos, movement, camera_model_pos);
  }
  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
    camera_model_rot = glm_rad(-yaw + 90.0);

    glm_vec3_cross(camera_front, up, movement);
    glm_vec3_normalize(movement);
    glm_vec3_scale(movement, cam_speed, movement);
    glm_vec3_add(camera_model_pos, movement, camera_model_pos);
  }
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, 1);
  }
  if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS &&
      glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) != GLFW_PRESS) {
    if (glfwGetTime() - last_push >= 0.125) {
      if (cur_frame == 39) {
        cur_frame = 0;
      }
      cur_frame++;
      last_push = glfwGetTime();
    }
  } else if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS &&
             glfwGetKey(window, GLFW_KEY_SPACE) != GLFW_PRESS &&
             cur_frame > 0) {
    if (glfwGetTime() - last_push >= 0.125) {
      cur_frame--;
      last_push = glfwGetTime();
    }
  }
  if (glfwGetKey(window, GLFW_KEY_TAB) == GLFW_PRESS) {
    if (toggled) {
      if (draw == 1) {
        draw = 0;
      } else {
        draw = 1;
      }
      toggled = 0;
    }
  } else {
    toggled = 1;
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

  camera_front[0] = sin(glm_rad(yaw));
  camera_front[2] = -cos(glm_rad(yaw));
  glm_vec3_normalize(camera_front);
}

void scroll_callback(GLFWwindow *window, double xoffset, double yoffset) {
  camera_offset[2] += yoffset;
  if (camera_offset[2] < -8.0) {
    camera_offset[2] = -8.0;
  }
  if (camera_offset[2] > -3.0) {
    camera_offset[2] = -3.0;
  }
}
