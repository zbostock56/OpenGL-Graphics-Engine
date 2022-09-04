#include <obj_preprocessor.h>

int preprocess_lines(LINE_BUFFER *lb) {
  FILE *file = fopen(lb->path, "w");
  if (file == NULL) {
    printf("Unable to open preprocessed file\n");
    return -1;
  }
  fprintf(file, "#PRE\n");

  verticies = malloc(sizeof(float) * 3 * VERTEX_BUFF_STARTING_LEN);
  if (verticies == NULL) {
    fclose(file);
    printf("Unable to allocate vertex buffer\n");
    return -1;
  }
  v_buff_len = VERTEX_BUFF_STARTING_LEN;
  v_len = 0;

  normals = malloc(sizeof(float) * 3 * NORMAL_BUFF_STARTING_LEN);
  if (normals == NULL) {
    fclose(file);
    free(verticies);
    printf("Unable to allocate normal buffer\n");
    return -1;
  }
  n_buff_len = NORMAL_BUFF_STARTING_LEN;
  n_len = 0;

  tex_coords = malloc(sizeof(float) * 2 * TEX_COORD_BUFF_STARTING_LEN);
  if (tex_coords == NULL) {
    fclose(file);
    free(verticies);
    free(normals);
    printf("Unable to allocate tex coord buffer\n");
    return -1;
  }
  t_buff_len = TEX_COORD_BUFF_STARTING_LEN;
  t_len = 0;

  vbo_index_combos = malloc(sizeof(int) * 3 * VBO_STARTING_LEN);
  if (vbo_index_combos == NULL) {
    fclose(file);
    free(verticies);
    free(normals);
    free(tex_coords);
    printf("Unable to allocate vbo index combos\n");
    return -1;
  }
  vbo_buff_len = VBO_STARTING_LEN;
  vbo_len = 0;

  char *cur_line = NULL;
  int status = 0;
  for (int i = 0; i < lb->len; i++) {
    cur_line = lb->buffer[i];

    if (cur_line[0] == 'f') {
      status = preprocess_face(file, cur_line + 2);
    } else {
      fprintf(file, "%s\n", cur_line);

      if (cur_line[0] == 'v' && cur_line[1] == 't') {
        sscanf(cur_line, "vt %f %f",
              &(tex_coords[t_len][0]),
              &(tex_coords[t_len][1])
            );
        t_len++;
        if (t_len == t_buff_len) {
          status = double_buffer((void **) &tex_coords, &t_buff_len,
                                 sizeof(float) * 2);
        }
      } else if (cur_line[0] == 'v' && cur_line[1] == 'n') {
        sscanf(cur_line, "vn %f %f %f",
              &(normals[n_len][0]),
              &(normals[n_len][1]),
              &(normals[n_len][2])
            );
        n_len++;
        if (n_len == n_buff_len) {
          status = double_buffer((void **) &normals, &n_buff_len,
                                 sizeof(float) * 3);
        }
      } else if (cur_line[0] == 'v') {
        sscanf(cur_line, "v %f %f %f",
              &(verticies[v_len][0]),
              &(verticies[v_len][1]),
              &(verticies[v_len][2])
            );
        v_len++;
        if (v_len == v_buff_len) {
          status = double_buffer((void **) &verticies, &v_buff_len,
                                 sizeof(float) * 3);
        }
      }
    }

    if (status != 0) {
      fclose(file);
      free(verticies);
      free(normals);
      free(tex_coords);
      free(vbo_index_combos);
      printf("Parse error at line %d\n", i);
      return -1;
    }
  }

  fclose(file);
  free_line_buffer(lb);

  free(verticies);
  v_buff_len = 0;
  v_len = 0;

  free(normals);
  n_buff_len = 0;
  n_len = 0;

  free(tex_coords);
  t_buff_len = 0;
  t_len = 0;

  free(vbo_index_combos);
  vbo_buff_len = 0;
  vbo_len = 0;

  return 0;
}

int preprocess_face(FILE *file, char *line) {
  FACE_VERT *face_list_head = NULL;
  FACE_VERT *face_list_tail = NULL;
  size_t num_verts = 0;

  int status = 0;
  //                      v   t   n
  int index_combo[3] = { -1, -1, -1 };
  int cur_attrib = 0;
  char *cur_num = line;
  int read_index = 0;

  int line_len = strlen(line) + 1;
  for (int i = 0; i < line_len; i++) {
    if (line[i] == '/') {
      line[i] = '\0';
      read_index = atoi(cur_num) - 1;
      cur_num = line + i + 1;

      if (read_index >= 0 && cur_attrib == 0) {
        if (read_index > v_len - 1) {
          printf("Preprocessor Error: Invalid vertex index\n");
          return -1;
        }
        index_combo[0] = read_index;
      } else if (cur_attrib == 1) {
        if (read_index > t_len - 1) {
          printf("Preprocessor Error: Invalid tex coord index\n");
          return -1;
        }
        index_combo[1] = read_index;
      } else if(read_index >= 0) {
        printf("Preprocessor Error: Invalid number of vertex attributes\n");
        return -1;
      }
      cur_attrib++;
    } else if (line[i] == ' ' || line[i] == '\0') {
      line[i] = '\0';
      read_index = atoi(cur_num) - 1;
      cur_num = line + i + 1;

      if (cur_attrib == 0) {
        if (read_index > v_len - 1) {
          printf("Preprocessor Error: Invalid vertex index\n");
          return -1;
        }
        index_combo[0] = read_index;
      } else {
        if (read_index > n_len - 1) {
          printf("Preprocessor Error:Invalid normal index\n");
          return -1;
        }
        index_combo[2] = read_index;
      }

      int found = -1;
      for (int i = 0; i < vbo_len && found == -1; i++) {
        if (vbo_index_combos[i][0] == index_combo[0] &&
            vbo_index_combos[i][1] == index_combo[1] &&
            vbo_index_combos[i][2] == index_combo[2]) {
          found = i;
        }
      }

      if (found == -1) {
        vbo_index_combos[vbo_len][0] = index_combo[0];
        vbo_index_combos[vbo_len][1] = index_combo[1];
        vbo_index_combos[vbo_len][2] = index_combo[2];
        found = vbo_len;
        vbo_len++;

        if (vbo_len == vbo_buff_len) {
          status = double_buffer((void **) &vbo_index_combos, &vbo_buff_len,
                                 sizeof(int) * 3);
        }

        if (status != 0) {
          printf("Preproccessor Error: Unable to realloc vbo indicies\n");
          return -1;
        }
      }

      if (face_list_head == NULL) {
        face_list_head = malloc(sizeof(FACE_VERT));
        face_list_head->index = found;
        face_list_head->prev = NULL;
        face_list_head->next = NULL;
        face_list_tail = face_list_head;
      } else {
        face_list_tail->next = malloc(sizeof(FACE_VERT));
        face_list_tail->next->prev = face_list_tail;
        face_list_tail = face_list_tail->next;

        face_list_tail->next = NULL;
        face_list_tail->index = found;
      }
      num_verts++;

      cur_attrib = 0;
      index_combo[0] = -1;
      index_combo[1] = -1;
      index_combo[2] = -1;
    }
  }
  face_list_tail->next = face_list_head;
  face_list_head->prev = face_list_tail;

  if (num_verts == 3) {
    int face_verts[3] = {
      face_list_head->prev->index,
      face_list_head->index,
      face_list_head->next->index
    };

    status = write_triangle(file, face_verts);

    face_list_tail->next = NULL;
    face_list_head->prev = NULL;
    while (face_list_head->next != NULL) {
      face_list_head = face_list_head->next;
      free(face_list_head->prev);
    }
    free(face_list_head);
  } else {
    status = triangulate_polygon(file, face_list_head, num_verts);
  }

  return status;
}

int triangulate_polygon(FILE *file, FACE_VERT *head, size_t num_verts) {
  float poly_normal[3] = {
    normals[vbo_index_combos[head->index][2]][0],
    normals[vbo_index_combos[head->index][2]][1],
    normals[vbo_index_combos[head->index][2]][2]
  };

  int verts_left = num_verts;
  int cur_triangle[3] = { -1, -1, -1 };

  FACE_VERT *cur_vert = head;
  FACE_VERT *temp = NULL;
  while (verts_left > 3) {
    cur_triangle[0] = cur_vert->prev->index;
    cur_triangle[1] = cur_vert->index;
    cur_triangle[2] = cur_vert->next->index;

    if (is_ear(cur_triangle, cur_vert, poly_normal) == 0) {
      write_triangle(file, cur_triangle);

      cur_vert->prev->next = cur_vert->next;
      cur_vert->next->prev = cur_vert->prev;
      temp = cur_vert;
      cur_vert = cur_vert->next;
      free(temp);
      verts_left--;
    } else {
      cur_vert = cur_vert->next;
    }
  }

  cur_triangle[0] = cur_vert->prev->index;
  cur_triangle[1] = cur_vert->index;
  cur_triangle[2] = cur_vert->next->index;
  write_triangle(file, cur_triangle);

  free(cur_vert->prev);
  free(cur_vert->next);
  free(cur_vert);

  return 0;
}

int is_ear(int *triangle, FACE_VERT *ref_vert, float *polygon_normal) {
  float *coords[3] = {
    verticies[vbo_index_combos[triangle[1]][0]],
    verticies[vbo_index_combos[triangle[0]][0]],
    verticies[vbo_index_combos[triangle[2]][0]]
  };

  float u[3] = {
    coords[1][0] - coords[0][0],
    coords[1][1] - coords[0][1],
    coords[1][2] - coords[0][2]
  };

  float v[3] = {
    coords[2][0] - coords[0][0],
    coords[2][1] - coords[0][1],
    coords[2][2] - coords[0][2]
  };

  float u_cross_v[3] = {
    (u[1] * v[2]) - (u[2] * v[1]),
    (u[2] * v[0]) - (u[0] * v[2]),
    (u[0] * v[1]) - (u[1] * v[0])
  };

  float mult = 0;
  if (u_cross_v[0] != 0) {
    mult = polygon_normal[0] / u_cross_v[0];
  } else if (u_cross_v[1] != 0) {
    mult = polygon_normal[1] / u_cross_v[1];
  } else if (u_cross_v[2] != 0) {
    mult = polygon_normal[2] / u_cross_v[2];
  } else {
    // U x V IS (0, 0, 0) NEED SPECIAL CONSIDERATION
    return -1;
  }

  for (int i = 0; i < 3; i++) {
    if (u_cross_v[i] * mult != polygon_normal[i]) {
      return -1;
    }
  }

  float a = 0.0;
  float b = 0.0;
  float p[3] = { 0.0, 0.0, 0.0 };

  FACE_VERT *cur_vert = ref_vert->next->next;
  while (cur_vert != ref_vert->prev) {
    p[0] = verticies[vbo_index_combos[cur_vert->index][0]][0] - coords[0][0];
    p[1] = verticies[vbo_index_combos[cur_vert->index][0]][1] - coords[0][1];
    p[2] = verticies[vbo_index_combos[cur_vert->index][0]][2] - coords[0][2];

/*
        P(x)    P(x)V(x)U(y) - P(y)U(x)V(x)
    a = ---  -  ---------------------------
        U(x)    U(x)V(x)U(y) - U(x)U(x)V(y)
*/
    a = (p[0]/u[0]) -( ((p[0]*v[0]*u[1]) - (p[1]*u[0]*v[0])) /
                       ((u[0]*v[0]*u[1]) - (u[0]*u[0]*v[1])) );

/*
        P(x)U(y) - P(y)U(x)
    b = -------------------
        V(x)U(y) - U(x)V(y)
*/
    b = ((p[0]*u[1]) - (p[1]*u[0])) / ((v[0]*u[1]) - (u[0]*v[1]));

    if (a < 0.0 || a > 1.0 || b < 0.0 || b > 1.0 || a + b > 1.0) {
      return -1;
    }

    cur_vert = cur_vert->next;
  }

  return 0;
}

int write_triangle(FILE *file, int *verticies) {
  fprintf(file, "f");

  int *index_combo = NULL;
  for (int i = 0; i < 3; i++) {
    index_combo = vbo_index_combos[verticies[i]];
    if (index_combo[0] == -1) {
      printf("Preprocessor Error: No vertex data found\n");
      return -1;
    } else {
      fprintf(file, " %d", index_combo[0] + 1);
    }

    if (index_combo[1] == -1 && index_combo[2] != -1) {
      fprintf(file, "//%d", index_combo[2] + 1);
    } else if (index_combo[1] != -1 && index_combo[2] == -1){
      fprintf(file, "/%d", index_combo[1] + 1);
    } else if (index_combo[1] != -1 && index_combo[2] != -1) {
      fprintf(file, "/%d/%d", index_combo[1] + 1, index_combo[2] + 1);
    }
  }
  fprintf(file, "\n");

  return 0;
}
