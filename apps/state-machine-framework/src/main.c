
// System includes
#include <stdio.h>

// 3rd party includes
#include <zephyr/kernel.h>
#include <zephyr/smf.h>

typedef enum { 
    S1,
    S1A,
    S1B,
    S2,
    S3,
} MainSm_State;

typedef struct {
    // This SM object must be first!
    struct smf_ctx ctx;
    bool doTransition;
    bool handleEvent;
} SmObj;

// Forward declaration of state table
static const struct smf_state l_states[];

int main(void) {
    SmObj obj;
    obj.doTransition = false;
    obj.handleEvent = false;
    smf_set_initial(SMF_CTX(&obj), &l_states[S1]);

    printf("Should transition to S1.\n");
    smf_run_state(SMF_CTX(&obj));

    obj.doTransition = true;
    printf("Should transition to S1A.\n");
    smf_run_state(SMF_CTX(&obj));

    obj.doTransition = false;
    obj.handleEvent = true;
    printf("Sending event to S1A, telling it to handle it.\n");
    smf_run_state(SMF_CTX(&obj));

    obj.doTransition = true;
    obj.handleEvent = false;
    printf("Telling S1A to transition to S1B.\n");
    smf_run_state(SMF_CTX(&obj));

    obj.doTransition = true;
    obj.handleEvent = false;
    printf("Telling S1B to transition back to S1A.\n");
    smf_run_state(SMF_CTX(&obj));

    obj.doTransition = false;
    obj.handleEvent = false;
    printf("Sending event to S1A, telling it not to handle it.\n");
    smf_run_state(SMF_CTX(&obj));

    return 0;
}

static void s1_entry(void *obj) {
    printf("S1 entry.\n");
}

static void s1_run(void *obj) {
    SmObj *smObj = (SmObj *)obj;
    printf("S1 run.\n");
    if (smObj->doTransition) {
        smf_set_state(SMF_CTX(smObj), &l_states[S1A]);
    }
}

static void s1_exit(void *obj) {
    printf("S1 exit.\n");
}

static void s1a_entry(void *obj) {
    printf("S1A entry.\n");
}

static void s1a_run(void *obj) {
    printf("S1A run.\n");
    SmObj *smObj = (SmObj *)obj;

    if (smObj->doTransition) {
        smf_set_state(SMF_CTX(smObj), &l_states[S1B]);
    }

    if (smObj->handleEvent) {
        printf("S1A handling event.\n");
        smf_set_handled(SMF_CTX(smObj));
    }
    else {
        printf("S1A not handling event.\n");
    }
}

static void s1a_exit(void *obj) {
    printf("S1A exit.\n");
}


static void s1b_entry(void *obj) {
    printf("S1B entry.\n");
}

static void s1b_run(void *obj) {
    printf("S1B run.\n");
    SmObj *smObj = (SmObj *)obj;

    if (smObj->doTransition) {
        smf_set_state(SMF_CTX(smObj), &l_states[S1A]);
    }
}

static void s1b_exit(void *obj) {
    printf("S1B exit.\n");
}

static void s2_entry(void *obj) {
    printf("S2 entry.\n");
}

static void s2_run(void *obj) {
    printf("S2 run.\n");
}

static void s2_exit(void *obj) {
    printf("S2 exit.\n");
}

static void s3_entry(void *obj) {
    printf("S3 entry.\n");
}

static void s3_run(void *obj) {
    printf("S3 run.\n");
}

static void s3_exit(void *obj) {
    printf("S3 exit.\n");
}

static const struct smf_state l_states[] = {
    [S1] = SMF_CREATE_STATE(s1_entry, s1_run, s1_exit, NULL, NULL),
    [S1A] = SMF_CREATE_STATE(s1a_entry, s1a_run, s1a_exit, &l_states[S1], NULL),
    [S1B] = SMF_CREATE_STATE(s1b_entry, s1b_run, s1b_exit, &l_states[S1], NULL),
    [S2] = SMF_CREATE_STATE(s2_entry, s2_run, s2_exit, NULL, NULL),
    [S3] = SMF_CREATE_STATE(s3_entry, s3_run, s3_exit, NULL, NULL),
};
