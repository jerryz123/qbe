#include "all.h"

extern char *rname[];

extern void emitins(Ins *i, Fn *fn, FILE *f);

/*

  Stack-frame layout:

  +=============+
  | varargs     |
  |  save area  |
  +-------------+
  |  saved ra   |
  |  saved fp   |
  +-------------+ <- fp
  |    ...      |
  | spill slots |
  |    ...      |
  +-------------+
  |    ...      |
  |   locals    |
  |    ...      |
  +-------------+
  |   padding   |
  +-------------+
  | callee-save |
  |  registers  |
  +=============+

*/

void
rvliw64_emitfn(Fn *fn, FILE *f)
{
	static int id0;
	int lbl, neg, off, frame, *pr, r;
	Blk *b, *s;
	Ins *i;

	emitfnlnk(fn->name, &fn->lnk, f);

	if (fn->vararg) {
		/* TODO: only need space for registers
		 * unused by named arguments
		 */
		fprintf(f, "\tadd sp, sp, -64\n");
		for (r=A0; r<=A7; r++)
			fprintf(f,
				"\tsd %s, %d(sp)\n",
				rname[r], 8 * (r - A0)
			);
	}
	fprintf(f, "\tsd fp, -16(sp)\n");
	fprintf(f, "\tsd ra, -8(sp)\n");
	fprintf(f, "\tadd fp, sp, -16\n");

	frame = (16 + 4 * fn->slot + 15) & ~15;
	for (pr=rv64_rclob; *pr>=0; pr++) {
		if (fn->reg & BIT(*pr))
			frame += 8;
	}
	frame = (frame + 15) & ~15;

	if (frame <= 2048)
		fprintf(f,
			"\tadd sp, sp, -%d\n",
			frame
		);
	else
		fprintf(f,
			"\tli t6, %d\n"
			"\tsub sp, sp, t6\n",
			frame
		);
	for (pr=rv64_rclob, off=0; *pr>=0; pr++) {
		if (fn->reg & BIT(*pr)) {
			fprintf(f,
				"\t%s %s, %d(sp)\n",
				*pr < FT0 ? "sd" : "fsd",
				rname[*pr], off
			);
			off += 8;
		}
	}

	for (lbl=0, b=fn->start; b; b=b->link) {
		if (lbl || b->npred > 1)
			fprintf(f, ".L%d:\n", id0+b->id);
		for (i=b->ins; i!=&b->ins[b->nins]; i++)
			emitins(i, fn, f);
		lbl = 1;
		switch (b->jmp.type) {
		case Jhlt:
			fprintf(f, "\tebreak\n");
			break;
		case Jret0:
			if (fn->dynalloc) {
				if (frame - 16 <= 2048)
					fprintf(f,
						"\tadd sp, fp, -%d\n",
						frame - 16
					);
				else
					fprintf(f,
						"\tli t6, %d\n"
						"\tsub sp, fp, t6\n",
						frame - 16
					);
			}
			for (pr=rv64_rclob, off=0; *pr>=0; pr++) {
				if (fn->reg & BIT(*pr)) {
					fprintf(f,
						"\t%s %s, %d(sp)\n",
						*pr < FT0 ? "ld" : "fld",
						rname[*pr], off
					);
					off += 8;
				}
			}
			fprintf(f,
				"\tadd sp, fp, %d\n"
				"\tld ra, 8(fp)\n"
				"\tld fp, 0(fp)\n"
				"\tret\n",
				16 + fn->vararg * 64
			);
			break;
		case Jjmp:
		Jmp:
			if (b->s1 != b->link)
				fprintf(f, "\tj .L%d\n", id0+b->s1->id);
			else
				lbl = 0;
			break;
		case Jjnz:
			neg = 0;
			if (b->link == b->s2) {
				s = b->s1;
				b->s1 = b->s2;
				b->s2 = s;
				neg = 1;
			}
			assert(isreg(b->jmp.arg));
			fprintf(f,
				"\tb%sz %s, .L%d\n",
				neg ? "ne" : "eq",
				rname[b->jmp.arg.val],
				id0+b->s2->id
			);
			goto Jmp;
		}
	}
	id0 += fn->nblk;
	elf_emitfnfin(fn->name, f);
}
