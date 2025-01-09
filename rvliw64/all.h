#include "../all.h"
#include "../rv64/reg.h"

/* targ.c */
extern int rv64_rsave[];
extern int rv64_rclob[];
extern Rv64Op rv64_op[];

/* abi.c */
extern bits rv64_retregs(Ref, int[2]);
extern bits rv64_argregs(Ref, int[2]);
extern void rv64_abi(Fn *);

/* isel.c */
void rvliw64_isel(Fn *);

/* emit.c */
void rvliw64_emitfn(Fn *, FILE *);
