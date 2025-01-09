#include "../all.h"
#include "reg.h"

MAKESURE(reg_not_tmp, FT11 < (int)Tmp0);

/* targ.c */
extern int rv64_rsave[];
extern int rv64_rclob[];
extern Rv64Op rv64_op[];

/* abi.c */
bits rv64_retregs(Ref, int[2]);
bits rv64_argregs(Ref, int[2]);
void rv64_abi(Fn *);

/* isel.c */
void rv64_isel(Fn *);

/* emit.c */
void rv64_emitfn(Fn *, FILE *);
