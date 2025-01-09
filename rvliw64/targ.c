#include "all.h"

#define RGLOB (BIT(FP) | BIT(SP) | BIT(GP) | BIT(TP) | BIT(RA))

extern int rv64_rsave[];

extern int rv64_memargs(int op);


Target T_rvliw64 = {
	.name = "rvliw64",
	.gpr0 = T0,
	.ngpr = NGPR,
	.fpr0 = FT0,
	.nfpr = NFPR,
	.rglob = RGLOB,
	.nrglob = 5,
	.rsave = rv64_rsave,
	.nrsave = {NGPS, NFPS},
	.retregs = rv64_retregs,
	.argregs = rv64_argregs,
	.memargs = rv64_memargs,
	.abi0 = elimsb,
	.abi1 = rv64_abi,
	.isel = rvliw64_isel,
	.emitfn = rvliw64_emitfn,
	.emitfin = elf_emitfin,
	.asloc = ".L",
};
