#define COMMONLIB_REMOVE_PREFIX
#define COMMONLIB_IMPLEMENTATION
#include "../commonlib.h"

typedef struct {
    int *items;
    size_t count;
    size_t capacity;
} My_Queue;


typedef struct {
    const char *name;
    int age;
} Person;

typedef struct {
    Person *items;
    size_t count;
    size_t capacity;
} Person_Queue;

int main(void) {

    My_Queue qu = {0};

    da_append(qu, 1);
    da_append(qu, 5);
    da_append(qu, 7);
    log_info("%zu", qu.count);

    for (int i = 0; i < qu.count; ++i) {
        log_info("[%d] = %d", i, qu.items[i]);
    }

    for (int i = 0; i < 10; ++i) {
        if (c_qu_has((C_Queue*)&qu, (void*)&i, sizeof(int))) {
            log_info("Queue has %d in it!", i);
        }
    }

    log_info("--------------------------------------------------");

    Person_Queue p_qu = {0};

    da_append(p_qu, ((Person) {
        .name = "alice",
        .age = 27,
    }));

    da_append(p_qu, ((Person) {
        .name = "bob",
        .age = 32,
    }));

    for (int i = 0; i < p_qu.count; ++i) {
        log_info("[%d] = {name: %s, age: %d}", i, p_qu.items[i].name, p_qu.items[i].age);
    }

    Person persons[] = {
        (Person) { "alice", 27 },
        (Person) { "bob", 32 },
        (Person) { "zack", 34 },
    };

//     for (int i = 0; i < ARRAY_LEN(persons); ++i) {
//         bool has = false;
//         qu_has(p_qu, persons[i], has);
// 
//         if (has) {
//             log_info("queue has {name: %s, age: %d}", persons[i].name, persons[i].age);
//         } else {
//             log_info("queue doens't {name: %s, age: %d}", persons[i].name, persons[i].age);
//         }
//     }

    int diff = C_MEMCMP(&(p_qu.items[0]), &(persons[0]), sizeof(Person));
    if (diff == 0) {
        log_info("Alice matches!");
    } else {
        log_info("Alice doesn't match!");
    }
    log_info("diff: %d", diff);

//     for (int i = 0; i < ARRAY_LEN(persons); ++i) {
//         if (c_qu_has((C_Queue*)&p_qu, &persons[i], sizeof(Person))) {
//             log_info("queue has {name: %s, age: %d}", persons[i].name, persons[i].age);
//         } else {
//             log_info("queue doens't {name: %s, age: %d}", persons[i].name, persons[i].age);
//         }
//             
//     }

    return 0;
}
