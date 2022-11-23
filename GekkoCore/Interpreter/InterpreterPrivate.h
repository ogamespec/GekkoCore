// Private interpreter stuff

#pragma once

#define IS_XER_SO       ((core->regs.spr[Gekko::SPR::XER] & GEKKO_XER_SO) != 0)
#define IS_XER_OV       ((core->regs.spr[Gekko::SPR::XER] & GEKKO_XER_OV) != 0)
#define IS_XER_CA       ((core->regs.spr[Gekko::SPR::XER] & GEKKO_XER_CA) != 0)

#define IS_NAN(n)       (((n) & 0x7ff0000000000000) == 0x7ff0000000000000 && ((n) & 0x000fffffffffffff) != 0)
#define IS_SNAN(n)      (((n) & 0x7ff0000000000000) == 0x7ff0000000000000 && ((n) & 0x000fffffffffffff) != 0 && ((n) & 0x0008000000000000) == 0)
#define SET_CRF(n, c)   (core->regs.cr = (core->regs.cr & (~(0xf0000000 >> (4 * n)))) | (c << (4 * (7 - n))))

#define COMPUTE_CR0(r)                                  \
{                                                       \
    (core->regs.cr = (core->regs.cr & 0x0fff'ffff)|     \
    (IS_XER_SO ? (GEKKO_CR0_SO) : (0)) |                \
    (((int32_t)(r) < 0) ? (GEKKO_CR0_LT) : (((int32_t)(r) > 0) ? (GEKKO_CR0_GT) : (GEKKO_CR0_EQ))));\
}

#define COMPUTE_CR1()                                   \
{                                                       \
    core->regs.cr = (core->regs.cr & 0xf0ff'ffff) | ((core->regs.fpscr & 0xf000'0000) >> 4);    \
}

#define FPRU(n) (core->regs.fpr[n].uval)
#define FPRD(n) (core->regs.fpr[n].dbl)
#define PS0(n)  (core->regs.fpr[n].dbl)
#define PS1(n)  (core->regs.ps1[n].dbl)
#define PS0U(n)  (core->regs.fpr[n].uval)
#define PS1U(n)  (core->regs.ps1[n].uval)
