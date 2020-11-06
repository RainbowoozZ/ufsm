#ifndef INCLUDE_UFSMM_MODEL_H_
#define INCLUDE_UFSMM_MODEL_H_

#include <stdint.h>
#include <uuid/uuid.h>
#include <stdbool.h>
#include <json.h>

enum ufsmm_transition_kind
{
    UFSMM_TRANSITION_EXTERNAL,
    UFSMM_TRANSITION_INTERNAL,
    UFSMM_TRANSITION_LOCAL,
};

enum ufsmm_state_kind
{
    UFSMM_STATE_NORMAL,
    UFSMM_STATE_INIT,
    UFSMM_STATE_FINAL,
    UFSMM_STATE_SHALLOW_HISTORY,
    UFSMM_STATE_DEEP_HISTORY,
    UFSMM_STATE_EXIT_POINT,
    UFSMM_STATE_ENTRY_POINT,
    UFSMM_STATE_JOIN,
    UFSMM_STATE_FORK,
    UFSMM_STATE_CHOICE,
    UFSMM_STATE_JUNCTION,
    UFSMM_STATE_TERMINATE,
};

enum ufsmm_action_kind
{
    UFSMM_ACTION_ACTION,
    UFSMM_ACTION_GUARD,
    UFSMM_ACTION_ENTRY,
    UFSMM_ACTION_EXIT,
};

enum ufsmm_side
{
    UFSMM_SIDE_NONE,
    UFSMM_SIDE_LEFT,
    UFSMM_SIDE_RIGHT,
    UFSMM_SIDE_TOP,
    UFSMM_SIDE_BOTTOM,
};

enum ufsmm_transition_vertice_kind
{
    UFSMM_TRANSITION_VERTICE_NONE,
    UFSMM_TRANSITION_VERTICE,
    UFSMM_TRANSITION_VERTICE_START,
    UFSMM_TRANSITION_VERTICE_END,
};

struct ufsmm_action
{
    uuid_t id;
    const char *name;
    enum ufsmm_action_kind kind;
    struct ufsmm_action *next;
};

struct ufsmm_action_ref
{
    uuid_t id;
    struct ufsmm_action *act;
    struct ufsmm_action_ref *next;
};

struct ufsmm_trigger
{
    uuid_t id;
    const char *name;
    struct ufsmm_trigger *next;
};

struct ufsmm_vertice
{
    double x;
    double y;
    struct ufsmm_vertice *next;
};

struct ufsmm_transition_state_ref
{
    struct ufsmm_state *state;
    double offset;
    enum ufsmm_side side;
};

struct ufsmm_coords
{
    double x;
    double y;
    double w;
    double h;
};

struct ufsmm_transition_state_condition
{
    struct ufsmm_state *state;
    bool positive;
    struct ufsmm_transition_state_condition *next;
};

struct ufsmm_transition
{
    uuid_t id;
    struct ufsmm_trigger *trigger;
    enum ufsmm_transition_kind kind;
    struct ufsmm_action_ref *action;
    struct ufsmm_action_ref *guard;
    struct ufsmm_transition_state_ref source;
    struct ufsmm_transition_state_ref dest;
    struct ufsmm_coords text_block_coords;
    struct ufsmm_vertice *vertices;
    struct ufsmm_transition_state_condition *state_conditions;
    bool focus;
    struct ufsmm_transition *prev;
    struct ufsmm_transition *next;
};

struct ufsmm_region
{
    uuid_t id;
    const char *name;
    bool off_page;
    double h;
    bool focus;
    bool draw_as_root;
    unsigned int depth;
    struct ufsmm_state *state;
    struct ufsmm_state *parent_state;
    struct ufsmm_state *last_state;
    struct ufsmm_region *prev;
    struct ufsmm_region *next;
};

struct ufsmm_state
{
    uuid_t id;
    const char *name;
    double x;
    double y;
    double w;
    double h;
    double region_y_offset;
    bool focus;
    bool resizeable;
    unsigned int branch_concurrency_count;
    enum ufsmm_state_kind kind;
    struct ufsmm_transition *transition;
    struct ufsmm_action_ref *entries;
    struct ufsmm_action_ref *exits;
    struct ufsmm_region *regions;
    struct ufsmm_region *parent_region;
    struct ufsmm_region *last_region;
    struct ufsmm_state *prev;
    struct ufsmm_state *next;
};

struct ufsmm_model
{
    json_object *jroot;
    struct ufsmm_region *root;
    struct ufsmm_action *entries; /* Global list of entry functions */
    struct ufsmm_action *exits;   /* Global list of exit functions  */
    struct ufsmm_action *guards;  /* Global list of guard functions */
    struct ufsmm_action *actions; /* Global list of action functions */
    struct ufsmm_trigger *triggers;
    const char *name;
    int version;
    unsigned int no_of_regions;
    unsigned int no_of_states;
};

int ufsmm_model_load(const char *filename, struct ufsmm_model **model);
int ufsmm_model_create(struct ufsmm_model **model, const char *name);
int ufsmm_model_write(const char *filename, struct ufsmm_model *model);
int ufsmm_model_free(struct ufsmm_model *model);
int ufsmm_model_add_action(struct ufsmm_model *model,
                          enum ufsmm_action_kind kind,
                          const char *name,
                          struct ufsmm_action **act);
int ufsmm_model_delete_action(struct ufsmm_model *model, uuid_t id);
int ufsmm_model_get_action(struct ufsmm_model *model, uuid_t id,
                          enum ufsmm_action_kind kind,
                          struct ufsmm_action **result);

int ufsmm_model_add_trigger(struct ufsmm_model *model, const char *name,
                           struct ufsmm_trigger **out);
int ufsmm_model_delete_trigger(struct ufsmm_model *model, uuid_t id);
int ufsmm_model_get_trigger(struct ufsmm_model *model, uuid_t id,
                           struct ufsmm_trigger **out);

int ufsmm_model_deserialize_coords(json_object *j_coords,
                                  struct ufsmm_coords *coords);

int ufsmm_model_calculate_max_orthogonal_regions(struct ufsmm_model *model);
int ufsmm_model_calculate_nested_region_depth(struct ufsmm_model *model);
int ufsmm_model_calculate_max_transitions(struct ufsmm_model *model);
int ufsmm_model_calculate_max_concurrent_states(struct ufsmm_model *model);

struct ufsmm_action* ufsmm_model_get_entries(struct ufsmm_model *model);
struct ufsmm_action* ufsmm_model_get_exits(struct ufsmm_model *model);
struct ufsmm_action* ufsmm_model_get_guards(struct ufsmm_model *model);
struct ufsmm_action* ufsmm_model_get_actions(struct ufsmm_model *model);
struct ufsmm_trigger* ufsmm_model_get_triggers(struct ufsmm_model *model);

struct ufsmm_state *ufsmm_model_get_state_from_uuid(struct ufsmm_model *model,
                                                  uuid_t id);

struct ufsmm_trigger * ufsmm_model_get_trigger_from_uuid(struct ufsmm_model *model,
                                                       uuid_t id);
int free_action_ref_list(struct ufsmm_action_ref *list);

/* Region api */
int ufsmm_add_region(struct ufsmm_state *state, bool off_page,
                     struct ufsmm_region **out);
int ufsmm_set_region_name(struct ufsmm_region *region, const char *name);

int ufsmm_region_append_state(struct ufsmm_region *r, struct ufsmm_state *state);

int ufsmm_region_serialize(struct ufsmm_region *region, json_object *state,
                         json_object **out);

int ufsmm_region_deserialize(json_object *j_r, struct ufsmm_state *state,
                            struct ufsmm_region **out);

int ufsmm_region_set_height(struct ufsmm_region *r, double h);
int ufsmm_region_get_height(struct ufsmm_region *r, double *h);

/* State api */

int ufsmm_delete_state(struct ufsmm_state *state);

int ufsmm_add_state(struct ufsmm_region *region, const char *name,
                    struct ufsmm_state **out);

int ufsmm_state_add_entry(struct ufsmm_model *model,
                         struct ufsmm_state *state,
                         uuid_t id,
                         uuid_t action_id);

int ufsmm_state_add_exit(struct ufsmm_model *model,
                        struct ufsmm_state *state,
                        uuid_t id,
                        uuid_t action_id);

int ufsmm_state_delete_entry(struct ufsmm_state *state, uuid_t id);
int ufsmm_state_delete_exit(struct ufsmm_state *state, uuid_t id);

int ufsmm_state_get_entries(struct ufsmm_state *state,
                           struct ufsmm_action_ref **list);
int ufsmm_state_get_exits(struct ufsmm_state *state,
                         struct ufsmm_action_ref **list);

int ufsmm_state_add_transition(struct ufsmm_state *source,
                              struct ufsmm_state *dest,
                              struct ufsmm_transition **transition);

int ufsmm_state_delete_transition(struct ufsmm_transition *transition);

int ufsmm_state_get_transitions(struct ufsmm_state *state,
                               struct ufsmm_transition **transitions);

int ufsmm_state_append_region(struct ufsmm_state *state, struct ufsmm_region *r);

int ufsmm_state_serialize(struct ufsmm_state *state, json_object *region,
                        json_object **out);

int ufsmm_state_deserialize(struct ufsmm_model *model,
                           json_object *j_state,
                           struct ufsmm_region *region,
                           struct ufsmm_state **out);

int ufsmm_state_set_size(struct ufsmm_state *s, double x, double y);
int ufsmm_state_set_xy(struct ufsmm_state *s, double x, double y);

int ufsmm_state_get_size(struct ufsmm_state *s, double *x, double *y);
int ufsmm_state_get_xy(struct ufsmm_state *s, double *x, double *y);


const char * ufsmm_model_name(struct ufsmm_model *model);

/* Transition API */
int ufsmm_transition_deserialize(struct ufsmm_model *model,
                                struct ufsmm_state *state,
                                json_object *j_object);

int ufsmm_transitions_serialize(struct ufsmm_state *state,
                              json_object *j_output);

int ufsmm_transition_set_trigger(struct ufsmm_model *model,
                                struct ufsmm_transition *transition,
                                struct ufsmm_trigger *trigger);

int ufsmm_transition_add_guard(struct ufsmm_model *model,
                              struct ufsmm_transition *transition,
                              uuid_t id,
                              uuid_t action_id);

int ufsmm_transition_delete_guard(struct ufsmm_transition *transition, uuid_t id);
struct ufsmm_action_ref *ufsmm_transition_get_guards(struct ufsmm_transition *t);

int ufsmm_transition_add_action(struct ufsmm_model *model,
                               struct ufsmm_transition *transition,
                               uuid_t id,
                               uuid_t action_id);

int ufsmm_transition_delete_action(struct ufsmm_transition *transition, uuid_t id);
struct ufsmm_action_ref *ufsmm_transition_get_actions(struct ufsmm_transition *t);

int ufsmm_transition_add_state_condition(struct ufsmm_model *model,
                                        struct ufsmm_transition *transition,
                                        uuid_t id,
                                        bool positive);

int ufsmm_transition_delete_state_condition(struct ufsmm_transition *transition,
                                            uuid_t id);

struct ufsmm_transition_state_condition *
ufsmm_transition_get_state_conditions(struct ufsmm_transition *t);

int ufsmm_transition_free(struct ufsmm_transition *transition);

#endif  // INCLUDE_UFSMM_MODEL_H_