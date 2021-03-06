/*
 * udd cbt.h file
 *
 * 03-Sep-89, Chuck Cranor
 *
 * V5.1C (see VERSION file for history)
 *
 */

/* ONLY useful to CBT routines... */
/* should not be used by external stuff */

#ifdef CBTMAIN
#define CBT_EXTERN
#else
#define CBT_EXTERN extern
#endif

struct slev {
int (*ms)();
int (*cs)();
};

extern const struct slev spell_fns[4];

CBT_EXTERN int monster_level, monster_type, min_monster_hits, m_str, m_arm, monster_hits, monster_hits_old;
CBT_EXTERN const char *mnam;
CBT_EXTERN int strike_force, monster_defeated;
CBT_EXTERN bool autoevade, gone, not_in_combat, monster_skips_a_turn;

void cbt_uhitm(int damage);
