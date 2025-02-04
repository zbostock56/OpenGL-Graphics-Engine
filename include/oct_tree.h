#include <stdio.h>
#include <float.h>
#include <cglm/vec3.h>
#include <cglm/mat4.h>
#include <cglm/affine.h>
#include <entity_str.h>

#define MAX_DEPTH (5)
#define OCT_TREE_STARTING_LEN (25)
#define BUFF_STARTING_LEN (10)

typedef enum {
  X_Y_Z = 0,
  X_Y_negZ = 1,
  X_negY_Z = 2,
  X_negY_negZ = 3,
  negX_Y_Z = 4,
  negX_Y_negZ = 5,
  negX_negY_Z = 6,
  negX_negY_negZ = 7,
  MULTIPLE = -1
} OCTANT;

typedef struct physics_object {
  ENTITY *entity;
  size_t collider_offset;
  size_t node_offset;
  size_t next_offset;
  size_t prev_offset;
} PHYS_OBJ;

typedef struct oct_tree_node {
  size_t head_offset;
  size_t tail_offset;
  int next_offset;
  int empty;
} OCT_NODE;

typedef struct oct_tree {
  OCT_NODE *node_buffer;
  PHYS_OBJ *data_buffer;
  size_t node_buff_len;
  size_t node_buff_size;
  size_t data_buff_len;
  size_t data_buff_size;
  unsigned int type;
} OCT_TREE;

typedef struct collision_result {
  PHYS_OBJ **list;
  size_t list_len;
  size_t list_buff_size;
} COLLISION_RES;

vec3 X = { 1.0, 0.0, 0.0 };
vec3 NEG_X = { -1.0, 0.0, 0.0 };
vec3 Y = { 0.0, 1.0, 0.0 };
vec3 NEG_Y = { 0.0, -1.0, 0.0 };
vec3 Z = { 0.0, 0.0, 1.0 };
vec3 NEG_Z = { 0.0, 0.0, -1.0 };

// FRONT FACING

OCT_TREE *init_tree();
int oct_tree_insert(OCT_TREE *tree, ENTITY *entity, size_t collider_offset);
int oct_tree_delete(OCT_TREE *tree, size_t obj_offset);
COLLISION_RES oct_tree_search(OCT_TREE *tree, COLLIDER *hit_box);
void free_oct_tree(OCT_TREE *tree);

// BACK FACING

int init_node(OCT_TREE *tree, OCT_NODE *parent);
int read_oct(OCT_TREE *tree, OCT_NODE *node, COLLISION_RES *res);
int read_all_children(OCT_TREE *tree, OCT_NODE *node, COLLISION_RES *res);
int append_buffer(OCT_TREE *tree, size_t node_offset, ENTITY *entity,
                  size_t collider_offset);
int add_to_list(OCT_TREE *tree, size_t obj_offset, size_t node_offset);
int remove_from_list(OCT_TREE *tree, size_t obj_offset);
OCTANT detect_octant(vec3 min_extent, vec3 max_extent, float *ebj_extents,
                     float *oct_len);
int double_buffer(void **buffer, size_t *buff_size, size_t unit_size);
void get_model_mat(ENTITY *entity, mat4 model);
int max_dot(vec3 *verts, unsigned int len, vec3 dir);
