#include <stdio.h>
#include "xbyak/xbyak.h"
#include <stdlib.h>
#include <string.h>
#include "cybozu/inttype.hpp"
#define NUM_OF_ARRAY(x) (sizeof(x) / sizeof(x[0]))

using namespace Xbyak;

const int bitEnd = 64;

const uint64 MMX = 1ULL << 0;
const uint64 _XMM = 1ULL << 1;
const uint64 _MEM = 1ULL << 2;
const uint64 _REG32 = 1ULL << 3;
const uint64 EAX = 1ULL << 4;
const uint64 IMM32 = 1ULL << 5;
const uint64 IMM8 = 1ULL << 6;
const uint64 _REG8 = 1ULL << 7;
const uint64 _REG16 = 1ULL << 8;
const uint64 NEG8 = 1ULL << 9;
const uint64 IMM16 = 1ULL << 10;
const uint64 NEG16 = 1ULL << 11;
const uint64 AX = 1ULL << 12;
const uint64 AL = 1ULL << 13;
const uint64 IMM_1 = 1ULL << 14;
const uint64 MEM8 = 1ULL << 15;
const uint64 MEM16 = 1ULL << 16;
const uint64 MEM32 = 1ULL << 17;
const uint64 ONE = 1ULL << 19;
const uint64 CL = 1ULL << 20;
const uint64 MEM_ONLY_DISP = 1ULL << 21;
const uint64 NEG32 = 1ULL << 23;
const uint64 _YMM = 1ULL << 24;
const uint64 VM32X_32 = 1ULL << 39;
const uint64 VM32X_64 = 1ULL << 40;
const uint64 VM32Y_32 = 1ULL << 41;
const uint64 VM32Y_64 = 1ULL << 42;
#ifdef XBYAK64
const uint64 _MEMe = 1ULL << 25;
const uint64 REG32_2 = 1ULL << 26; // r8d, ...
const uint64 REG16_2 = 1ULL << 27; // r8w, ...
const uint64 REG8_2 = 1ULL << 28; // r8b, ...
const uint64 REG8_3 = 1ULL << 29; // spl, ...
const uint64 _REG64 = 1ULL << 30; // rax, ...
const uint64 _REG64_2 = 1ULL << 31; // r8, ...
const uint64 RAX = 1ULL << 32;
const uint64 _XMM2 = 1ULL << 33;
const uint64 _YMM2 = 1ULL << 34;
const uint64 VM32X = VM32X_32 | VM32X_64;
const uint64 VM32Y = VM32Y_32 | VM32Y_64;
#else
const uint64 _MEMe = 0;
const uint64 REG32_2 = 0;
const uint64 REG16_2 = 0;
const uint64 REG8_2 = 0;
const uint64 REG8_3 = 0;
const uint64 _REG64 = 0;
const uint64 _REG64_2 = 0;
const uint64 RAX = 0;
const uint64 _XMM2 = 0;
const uint64 _YMM2 = 0;
const uint64 VM32X = VM32X_32;
const uint64 VM32Y = VM32Y_32;
#endif
const uint64 REG64 = _REG64 | _REG64_2 | RAX;
const uint64 REG32 = _REG32 | REG32_2 | EAX;
const uint64 REG16 = _REG16 | REG16_2 | AX;
const uint64 REG32e = REG32 | REG64;
const uint64 REG8 = _REG8 | REG8_2|AL;
const uint64 MEM = _MEM | _MEMe;
const uint64 MEM64 = 1ULL << 35;
const uint64 ST0 = 1ULL << 36;
const uint64 STi = 1ULL << 37;
const uint64 IMM_2 = 1ULL << 38;
const uint64 IMM = IMM_1 | IMM_2;
const uint64 XMM = _XMM | _XMM2;
const uint64 YMM = _YMM | _YMM2;
const uint64 K = 1ULL << 43;
const uint64 _ZMM = 1ULL << 44;
const uint64 _ZMM2 = 1ULL << 45;
#ifdef XBYAK64
const uint64 ZMM = _ZMM | _ZMM2;
const uint64 _YMM3 = 1ULL << 46;
#else
const uint64 ZMM = _ZMM;
const uint64 _YMM3 = 0;
#endif
const uint64 K2 = 1ULL << 47;
const uint64 ZMM_SAE = 1ULL << 48;
const uint64 ZMM_ER = 1ULL << 49;
#ifdef XBYAK64
const uint64 _XMM3 = 1ULL << 50;
#endif
const uint64 XMM_SAE = 1ULL << 51;
#ifdef XBYAK64
const uint64 XMM_KZ = 1ULL << 52;
const uint64 YMM_KZ = 1ULL << 53;
const uint64 ZMM_KZ = 1ULL << 54;
#else
const uint64 XMM_KZ = 0;
const uint64 YMM_KZ = 0;
const uint64 ZMM_KZ = 0;
#endif
const uint64 MEM_K = 1ULL << 55;
const uint64 M_1to2 = 1ULL << 56;
const uint64 M_1to4 = 1ULL << 57;
const uint64 M_1to8 = 1ULL << 58;
const uint64 M_1to16 = 1ULL << 59;
const uint64 XMM_ER = 1ULL << 60;
const uint64 M_xword = 1ULL << 61;
const uint64 M_yword = 1ULL << 62;
const uint64 MY_1to4 = 1ULL << 18;

const uint64 NOPARA = 1ULL << (bitEnd - 1);

class Test {
	Test(const Test&);
	void operator=(const Test&);
	const bool isXbyak_;
	int funcNum_;
	// check all op1, op2, op3
	void put(const std::string& nm, uint64 op1 = NOPARA, uint64 op2 = NOPARA, uint64 op3 = NOPARA, uint64 op4 = NOPARA) const
	{
		for (int i = 0; i < bitEnd; i++) {
			if ((op1 & (1ULL << i)) == 0) continue;
			for (int j = 0; j < bitEnd; j++) {
				if ((op2 & (1ULL << j)) == 0) continue;
				for (int k = 0; k < bitEnd; k++) {
					if ((op3 & (1ULL << k)) == 0) continue;
					for (int s = 0; s < bitEnd; s++) {
						if ((op4 & (1ULL << s)) == 0) continue;
						printf("%s ", nm.c_str());
						if (isXbyak_) printf("(");
						if (!(op1 & NOPARA)) printf("%s", get(1ULL << i));
						if (!(op2 & NOPARA)) printf(", %s", get(1ULL << j));
						if (!(op3 & NOPARA)) printf(", %s", get(1ULL << k));
						if (!(op4 & NOPARA)) printf(", %s", get(1ULL << s));
						if (isXbyak_) printf("); dump();");
						printf("\n");
					}
				}
			}
		}
	}
	void put(const char *nm, uint64 op, const char *xbyak, const char *nasm) const
	{
		for (int i = 0; i < bitEnd; i++) {
			if ((op & (1ULL << i)) == 0) continue;
			printf("%s ", nm);
			if (isXbyak_) printf("(");
			if (!(op & NOPARA)) printf("%s", get(1ULL << i));
			printf(", %s", isXbyak_ ? xbyak : nasm);
			if (isXbyak_) printf("); dump();");
			printf("\n");
		}
	}
	void put(const char *nm, const char *xbyak, const char *nasm = 0, uint64 op = NOPARA) const
	{
		if (nasm == 0) nasm = xbyak;
		for (int i = 0; i < bitEnd; i++) {
			if ((op & (1ULL << i)) == 0) continue;
			printf("%s ", nm);
			if (isXbyak_) printf("(");
			printf("%s ", isXbyak_ ? xbyak : nasm);
			if (!(op & NOPARA)) printf(", %s", get(1ULL << i));
			if (isXbyak_) printf("); dump();");
			printf("\n");
		}
	}
	const char *get(uint64 type) const
	{
		int idx = (rand() / 31) & 7;
		if (type == ST0) {
			return "st0";
		}
		if (type == STi) {
			return "st2";
		}
		switch (type) {
		case MMX:
			{
				static const char MmxTbl[][4] = {
					"mm0", "mm1", "mm2", "mm3", "mm4", "mm5", "mm6", "mm7"
				};
				return MmxTbl[idx];
			}
		case _XMM:
			{
				static const char tbl[][6] = {
					"xmm0", "xmm1", "xmm2", "xmm3", "xmm4", "xmm5", "xmm6", "xmm7",
				};
				return tbl[idx];
			}
		case _YMM:
			{
				static const char tbl[][6] = {
					"ymm0", "ymm1", "ymm2", "ymm3", "ymm4", "ymm5", "ymm6", "ymm7"
				};
				return tbl[idx];
			}
		case _ZMM:
			{
				static const char tbl[][6] = {
					"zmm0", "zmm1", "zmm2", "zmm3", "zmm4", "zmm5", "zmm6", "zmm7"
				};
				return tbl[idx];
			}
#ifdef XBYAK64
		case _XMM2:
			{
				static const char tbl[][6] = {
					"xmm8", "xmm9", "xmm10", "xmm11", "xmm12", "xmm13", "xmm14", "xmm15"
				};
				return tbl[idx];
			}
		case _XMM3:
			{
				static const char tbl[][6] = {
					"xmm16", "xmm17", "xmm18", "xmm19", "xmm20", "xmm21", "xmm22", "xmm23"
				};
				return tbl[idx];
			}
		case _YMM2:
			{
				static const char tbl[][6] = {
					"ymm8", "ymm9", "ymm10", "ymm11", "ymm12", "ymm13", "ymm14", "ymm15",
				};
				return tbl[idx];
			}
		case _YMM3:
			{
				static const char tbl[][6] = {
					"ymm16", "ymm17", "ymm18", "ymm19", "ymm20", "ymm21", "ymm22", "ymm23",
				};
				return tbl[idx];
			}
		case _ZMM2:
			{
				static const char tbl[][6] = {
					"zmm8", "zmm9", "zmm10", "zmm11", "zmm28", "zmm29", "zmm30", "zmm31",
				};
				return tbl[idx];
			}
#endif
		case _MEM:
			return isXbyak_ ? "ptr[eax+ecx+64]" : "[eax+ecx+64]"; // QQQ
//			return isXbyak_ ? "ptr[eax+ecx+6]" : "[eax+ecx+6]";
		case _MEMe:
			{
				static int ccc = 1;
#ifdef USE_YASM
				ccc++;
#endif
				if (ccc & 1) {
					return isXbyak_ ? "ptr[rdx+r15+0x12]" : "[rdx+r15+0x12]";
				} else {
					return isXbyak_ ? "ptr[rip - 0x13456+1-3]" : "[rip - 0x13456+1-3]";
				}
			}
		case MEM8:
			return "byte [eax+edx]";
		case MEM16:
			return "word [esi]";
		case MEM32:
			return "dword [ebp*2]";
		case MEM64:
			return "qword [eax+ecx*8]";
		case MEM_ONLY_DISP:
			return isXbyak_ ? "ptr[(void*)0x123]" : "[0x123]";
		case _REG16: // not ax
			{
				static const char Reg16Tbl[][4] = {
					"ax", "cx", "dx", "bx", "sp", "bp", "si", "di"
				};
				return Reg16Tbl[(idx % 7) + 1];
			}
		case _REG8: // not al
			{
				static const char Reg8Tbl[][4] = {
#ifdef XBYAK64 // QQQ
					"al", "cl", "dl", "bl", "al", "cl", "dl", "bl"
#else
					"al", "cl", "dl", "bl", "ah", "ch", "dh", "bh"
#endif
				};
				return Reg8Tbl[(idx % 7) + 1];
			}
		case _REG32: // not eax
			{
				static const char Reg32Tbl[][4] = {
					"eax", "ecx", "edx", "ebx", "esp", "ebp", "esi", "edi"
				};
				return Reg32Tbl[(idx % 7) + 1];
			}
#ifdef XBYAK64
		case _REG64: // not rax
			{
				static const char Reg64Tbl[][4] = {
					"rax", "rcx", "rdx", "rbx", "rsp", "rbp", "rsi", "rdi"
				};
				return Reg64Tbl[(idx % 7) + 1];
			}
		case _REG64_2:
			{
				static const char Reg64_2Tbl[][4] = {
					"r8", "r9", "r10", "r11", "r12", "r13", "r14", "r15"
				};
				return Reg64_2Tbl[idx];
			}
		case REG32_2:
			{
				static const char Reg32eTbl[][5] = {
					"r8d", "r9d", "r10d", "r11d", "r12d", "r13d", "r14d", "r15d"
				};
				return Reg32eTbl[idx];
			}
		case REG16_2:
			{
				static const char Reg16eTbl[][5] = {
					"r8w", "r9w", "r10w", "r11w", "r12w", "r13w", "r14w", "r15w"
				};
				return Reg16eTbl[idx];
			}
		case REG8_2:
			{
				static const char Reg8_2Tbl[][5] = {
					"r8b", "r9b", "r10b", "r11b", "r12b", "r13b", "r14b", "r15b"
				};
				return Reg8_2Tbl[idx];
			}
		case REG8_3:
			{
				static const char Reg8_3Tbl[][5] = {
					"spl", "bpl", "sil", "dil", "spl", "bpl", "sil", "dil"
				};
				return Reg8_3Tbl[idx];
			}
		case RAX:
			return "rax";
#endif
		case EAX:
			return "eax";
		case AX:
			return "ax";
		case AL:
			return "al";
		case CL:
			return "cl";
		case ONE:
			return "1";
		case IMM32:
			return isXbyak_ ? "12345678" : "dword 12345678";
		case IMM16:
			return isXbyak_ ? "1000" : "word 1000";
		case IMM8:
			return isXbyak_ ? "4" : "byte 4";
		case NEG8:
			return isXbyak_ ? "-30" : "byte -30";
		case NEG16:
			return isXbyak_ ? "-1000" : "word -1000";
		case NEG32:
			return isXbyak_ ? "-100000" : "dword -100000";
		case IMM_1:
			return "4";
		case IMM_2:
			return isXbyak_ ? "0xda" : "0xda";
		case VM32X_32:
			return isXbyak_ ? "ptr [ebp+4+xmm1*8]" : "[ebp+4+xmm1*8]";
		case VM32X_64:
			return isXbyak_ ? "ptr [12345+xmm13*2]" : "[12345+xmm13*2]";
		case VM32Y_32:
			return isXbyak_ ? "ptr [ymm4]" : "[ymm4]";
		case VM32Y_64:
			return isXbyak_ ? "ptr [12345+ymm13*2+r13]" : "[12345+ymm13*2+r13]";
		case M_1to2: return isXbyak_ ? "ptr_b [eax+32]" : "[eax+32]{1to2}";
		case M_1to4: return isXbyak_ ? "ptr_b [eax+32]" : "[eax+32]{1to4}";
		case M_1to8: return isXbyak_ ? "ptr_b [eax+32]" : "[eax+32]{1to8}";
		case M_1to16: return isXbyak_ ? "ptr_b [eax+32]" : "[eax+32]{1to16}";

		case M_xword: return isXbyak_ ? "ptr [eax+32]" : "oword [eax+32]";
		case M_yword: return isXbyak_ ? "yword [eax+32]" : "yword [eax+32]";
		case MY_1to4: return isXbyak_ ? "yword_b [eax+32]" : "[eax+32]{1to4}";
		case K:
			{
				static const char kTbl[][5] = {
					"k1", "k2", "k3", "k4", "k5", "k6", "k7",
				};
				return kTbl[idx % 7];
			}
		case K2:
			return isXbyak_ ? "k3 | k5" : "k3{k5}";
#ifdef XBYAK64
		case XMM_SAE:
			return isXbyak_ ? "xmm25 | T_sae" : "xmm25, {sae}";
		case ZMM_SAE:
			return isXbyak_ ? "zmm25 | T_sae" : "zmm25, {sae}";
		case XMM_ER:
			return isXbyak_ ? "xmm4 | T_rd_sae" : "xmm4, {rd-sae}";
		case ZMM_ER:
			return isXbyak_ ? "zmm20 | T_rd_sae" : "zmm20, {rd-sae}";
		case XMM_KZ:
			return isXbyak_ ? "xmm5 | k5" : "xmm5{k5}";
		case YMM_KZ:
			return isXbyak_ ? "ymm2 |k3|T_z" : "ymm2{k3}{z}";
		case ZMM_KZ:
			return isXbyak_ ? "zmm7|k1" : "zmm7{k1}";
		case MEM_K:
			return isXbyak_ ? "ptr [rax] | k1" : "[rax]{k1}";
#else
		case XMM_SAE:
			return isXbyak_ ? "xmm5 | T_sae" : "xmm5, {sae}";
		case ZMM_SAE:
			return isXbyak_ ? "zmm5 | T_sae" : "zmm5, {sae}";
		case XMM_ER:
			return isXbyak_ ? "xmm30 | T_rd_sae" : "xmm30, {rd-sae}";
		case ZMM_ER:
			return isXbyak_ ? "zmm2 | T_rd_sae" : "zmm2, {rd-sae}";
		case MEM_K:
			return isXbyak_ ? "ptr [eax] | k1" : "[eax]{k1}";
#endif
		}
		return 0;
	}
public:
	Test(bool isXbyak)
		: isXbyak_(isXbyak)
		, funcNum_(1)
	{
		if (!isXbyak_) return;
		printf("%s",
			"    void gen0()\n"
			"    {\n");
	}
	/*
		gcc and vc give up to compile this source,
		so I split functions.
	*/
	void separateFunc()
	{
		if (!isXbyak_) return;
		printf(
			"    }\n"
			"    void gen%d()\n"
			"    {\n", funcNum_++);
	}
	~Test()
	{
		if (!isXbyak_) return;
		printf("%s",
			"    }\n"
			"    void gen()\n"
			"    {\n");
		for (int i = 0; i < funcNum_; i++) {
			printf(
			"        gen%d();\n", i);
		}
		printf(
			"    }\n");
	}
	void put()
	{
		putAVX512();
	}
	void putOpmask()
	{
		{
			const char *tbl[] = {
				"kadd",
				"kand",
				"kandn",
				"kor",
				"kxnor",
				"kxor",
			};
			for (size_t i = 0; i < NUM_OF_ARRAY(tbl); i++) {
				std::string name = tbl[i];
				put(name + "b", K, K, K);
				put(name + "w", K, K, K);
				put(name + "q", K, K, K);
				put(name + "d", K, K, K);
			}
			put("kunpckbw", K, K, K);
			put("kunpckwd", K, K, K);
			put("kunpckdq", K, K, K);
		}
		{
			const char *tbl[] = {
				"knot",
				"kortest",
				"ktest",
			};
			for (size_t i = 0; i < NUM_OF_ARRAY(tbl); i++) {
				std::string name = tbl[i];
				put(name + "b", K, K);
				put(name + "w", K, K);
				put(name + "q", K, K);
				put(name + "d", K, K);
			}
		}
		{
			const char *tbl[] = {
				"kshiftl",
				"kshiftr",
			};
			for (size_t i = 0; i < NUM_OF_ARRAY(tbl); i++) {
				std::string name = tbl[i];
				put(name + "b", K, K, IMM8);
				put(name + "w", K, K, IMM8);
				put(name + "q", K, K, IMM8);
				put(name + "d", K, K, IMM8);
			}
		}
		put("kmovw", K, K | MEM | REG32);
		put("kmovq", K, K | MEM);
		put("kmovb", K, K | MEM | REG32);
		put("kmovd", K, K | MEM | REG32);

		put("kmovw", MEM | REG32, K);
		put("kmovq", MEM, K);
		put("kmovb", MEM | REG32, K);
		put("kmovd", MEM | REG32, K);
#ifdef XBYAK64
		put("kmovq", K, REG64);
		put("kmovq", REG64, K);
#endif
	}
	void put_vaddpd(const char *r1, const char *r2, const char *r3, int kIdx = 0, bool z = false, int sae = 0)
	{
		std::string modifier;
		char pk[16] = "";
		const char *pz = "";
		const char *saeTblXbyak[] = { "", "|T_rn_sae", "|T_rd_sae", "|T_ru_sae", "|T_rz_sae" };
		const char *saeTblNASM[] = { "", ",{rn-sae}", ",{rd-sae}", ",{ru-sae}", ",{rz-sae}" };
		if (isXbyak_) {
			if (kIdx) CYBOZU_SNPRINTF(pk, sizeof(pk), "|k%d", kIdx);
			if (z) pz = "|T_z";
			printf("vaddpd(%s%s%s, %s, %s%s); dump();\n", r1, pk, pz, r2, r3, saeTblXbyak[sae]);
		} else {
			if (kIdx) CYBOZU_SNPRINTF(pk, sizeof(pk), "{k%d}", kIdx);
			if (z) pz = "{z}";
			printf("vaddpd %s%s%s, %s, %s%s\n", r1, pk, pz, r2, r3, saeTblNASM[sae]);
		}
	}
	void putCombi()
	{
		const char *xTbl[] = {
			"xmm2",
#ifdef XBYAK64
			"xmm8", "xmm31"
#else
			"xmm5", "xmm6"
#endif
		};
		const char *yTbl[] = {
			"ymm0",
#ifdef XBYAK64
			"ymm15", "ymm31"
#else
			"ymm4", "ymm2"
#endif
		};
		const char *zTbl[] = {
			"zmm1",
#ifdef XBYAK64
			"zmm9", "zmm30"
#else
			"zmm3", "zmm7"
#endif
		};
		const size_t N = NUM_OF_ARRAY(zTbl);
		for (size_t i = 0; i < N; i++) {
			for (size_t j = 0; j < N; j++) {
				separateFunc();
				for (size_t k = 0; k < N; k++) {
#ifdef XBYAK64
					for (int kIdx = 0; kIdx < 8; kIdx++) {
						for (int z = 0; z < 2; z++) {
							put_vaddpd(xTbl[i], xTbl[j], xTbl[k], kIdx, z == 1);
							put_vaddpd(yTbl[i], yTbl[j], yTbl[k], kIdx, z == 1);
							for (int sae = 0; sae < 5; sae++) {
								put_vaddpd(zTbl[i], zTbl[j], zTbl[k], kIdx, z == 1, sae);
							}
						}
					}
#else
					put_vaddpd(xTbl[i], xTbl[j], xTbl[k]);
					put_vaddpd(yTbl[i], yTbl[j], yTbl[k]);
					for (int sae = 0; sae < 5; sae++) {
						put_vaddpd(zTbl[i], zTbl[j], zTbl[k], sae);
					}
#endif
				}
			}
		}
		put("vaddpd", XMM, XMM, _MEM);
		put("vaddpd", YMM, YMM, _MEM);
		put("vaddpd", ZMM, ZMM, _MEM);
	}
	void putCmpK()
	{
		{
			const struct Tbl {
				const char *name;
				bool supportYMM;
			} tbl[] = {
				{ "vcmppd", true },
				{ "vcmpps", true },
				{ "vcmpsd", false },
				{ "vcmpss", false },
			};
			for (size_t i = 0; i < NUM_OF_ARRAY(tbl); i++) {
				const Tbl *p = &tbl[i];
				put(p->name, K, _XMM, _XMM | MEM, IMM8);
				if (!p->supportYMM) continue;
				put(p->name, K, _YMM, _YMM | MEM, IMM8);
				put(p->name, K, _ZMM, _ZMM | MEM, IMM8);
			}
		}
		put("vcmppd", K2, ZMM, ZMM_SAE, IMM);
#ifdef XBYAK64
		{
			const struct Tbl {
				const char *name;
			} tbl[] = {
				{ "vcomisd" },
				{ "vcomiss" },
				{ "vucomisd" },
				{ "vucomiss" },
			};
			for (size_t i = 0; i < NUM_OF_ARRAY(tbl); i++) {
				const Tbl *p = &tbl[i];
				put(p->name, XMM | _XMM3, XMM_SAE | XMM | MEM);
			}
		}
		put("vcomiss", _XMM3, XMM | MEM);
		put("vcomiss", XMM, XMM_SAE);
#endif
	}
	void putBroadcastSub(int idx, int disp)
	{
#ifdef XBYAK64
		const char *a = "rax";
#else
		const char *a = "eax";
#endif
		if (isXbyak_) {
			printf("vaddpd(zmm%d, zmm1, ptr_b[%s+%d]);dump();\n", idx, a, disp);
			printf("vaddpd(ymm%d, ymm1, ptr_b[%s+%d]);dump();\n", idx, a, disp);
			printf("vaddpd(xmm%d, xmm1, ptr_b[%s+%d]);dump();\n", idx, a, disp);
		} else {
			printf("vaddpd zmm%d, zmm1, [%s+%d]{1to8}\n", idx, a, disp);
			printf("vaddpd ymm%d, ymm1, [%s+%d]{1to4}\n", idx, a, disp);
			printf("vaddpd xmm%d, xmm1, [%s+%d]{1to2}\n", idx, a, disp);
		}
	}
	void putBroadcast()
	{
		for (int i = 0; i < 9; i++) {
			putBroadcastSub(0, i);
#ifdef XBYAK64
			putBroadcastSub(10, i);
			putBroadcastSub(20, i);
#endif
		}
		put("vpbroadcastb", XMM_KZ | ZMM_KZ, REG8 | _MEM);
		put("vpbroadcastw", XMM_KZ | ZMM_KZ, REG16 | _MEM);
		put("vpbroadcastd", XMM_KZ | ZMM_KZ, REG32 | _MEM);
#ifdef XBYAK64
		put("vpbroadcastq", XMM_KZ | ZMM_KZ, REG64 | _MEM);
#endif
		{
			const char *tbl[] = {
				"vpbroadcastb",
				"vpbroadcastw",
				"vpbroadcastd",
				"vpbroadcastq",
			};
			for (size_t i = 0; i < NUM_OF_ARRAY(tbl); i++) {
				put(tbl[i], XMM_KZ | ZMM_KZ, _XMM | _MEM);
			}
		}
		put("vbroadcasti32x2", XMM_KZ | YMM_KZ | ZMM_KZ, _XMM | _MEM);
		put("vbroadcasti32x4", YMM_KZ | ZMM_KZ, _MEM);
		put("vbroadcasti64x2", YMM_KZ | ZMM_KZ, _MEM);
		put("vbroadcasti32x8", ZMM_KZ, _MEM);
		put("vbroadcasti64x4", ZMM_KZ, _MEM);
	}
	void putMisc1()
	{
		put("vmaskmovps", XMM, XMM, MEM);
		put("vmaskmovps", YMM, YMM, MEM);

		put("vmaskmovpd", YMM, YMM, MEM);
		put("vmaskmovpd", XMM, XMM, MEM);

		put("vmaskmovps", MEM, XMM, XMM);
		put("vmaskmovpd", MEM, XMM, XMM);

		put("vbroadcastf128", YMM, MEM);
		put("vbroadcasti128", YMM, MEM);
		put("vbroadcastsd", YMM|_YMM3, XMM|MEM);
		put("vbroadcastsd", ZMM, XMM|MEM);
		{
			const char *tbl[] = {
				"vbroadcastss",
				"vpbroadcastb",
				"vpbroadcastw",
				"vpbroadcastd",
				"vpbroadcastq",
			};
			for (size_t i = 0; i < NUM_OF_ARRAY(tbl); i++) {
				put(tbl[i], XMM | YMM | ZMM, XMM|MEM);
			}
		}

		put("vinsertf128", YMM, YMM, XMM | MEM, IMM8);
		put("vinserti128", YMM, YMM, XMM | MEM, IMM8);
		put("vperm2f128", YMM, YMM, YMM | MEM, IMM8);
		put("vperm2i128", YMM, YMM, YMM | MEM, IMM8);

		{
			const char *tbl[] = {
				"vpmaskmovd", "vpmaskmovq"
			};
			for (size_t i = 0; i < NUM_OF_ARRAY(tbl); i++) {
				const char *name = tbl[i];
				put(name, XMM, XMM, MEM);
				put(name, YMM, YMM, MEM);
				put(name, MEM, XMM, XMM);
				put(name, MEM, YMM, YMM);
			}
		}
		{
			const char *tbl[] = {
				"vpermd", "vpermps",
			};
			for (size_t i = 0; i < NUM_OF_ARRAY(tbl); i++) {
				const char *name = tbl[i];
				put(name, YMM, YMM, YMM | MEM);
			}
		}
		{
			const char *tbl[] = {
				"vpermq", "vpermpd",
			};
			for (size_t i = 0; i < NUM_OF_ARRAY(tbl); i++) {
				const char *name = tbl[i];
				put(name, YMM, YMM | MEM, IMM8);
			}
		}
		put("vpextrw", REG32e | MEM, XMM, IMM); // nasm is ok, yasm generate redundant code
	}
	void putAVX512_M_X()
	{
		const char *tbl[] = {
			"vmovapd",
			"vmovaps",
			"vmovupd",
			"vmovups",
		};
		for (size_t i = 0; i < NUM_OF_ARRAY(tbl); i++) {
			const char *name = tbl[i];
			put(name, MEM, ZMM);
			put(name, ZMM, MEM);
#ifdef XBYAK64
			put(name, MEM, _XMM3);
			put(name, _XMM3, MEM);
#endif
		}
	}
	void put_vmov()
	{
#ifdef XBYAK64
		put("vmovd", _XMM3, MEM|REG32);
		put("vmovd", MEM|REG32, _XMM3);
		put("vmovq", _XMM3, MEM|REG64|XMM);
		put("vmovq", MEM|REG64|XMM, _XMM3);
		put("vmovhlps", _XMM3, _XMM3, _XMM3);
		put("vmovlhps", _XMM3, _XMM3, _XMM3);
		put("vmovntdqa", _XMM3|_YMM3|ZMM, MEM);
		put("vmovntdq", MEM, _XMM3 | _YMM3 | ZMM);
		put("vmovntpd", MEM, _XMM3 | _YMM3 | ZMM);
		put("vmovntps", MEM, _XMM3 | _YMM3 | ZMM);

		put("vmovsd", XMM_KZ, _XMM3, _XMM3);
		put("vmovsd", XMM_KZ, MEM);
		put("vmovsd", MEM_K, XMM);
		put("vmovss", XMM_KZ, _XMM3, _XMM3);
		put("vmovss", XMM_KZ, MEM);
		put("vmovss", MEM_K, XMM);

		put("vmovshdup", _ZMM, _ZMM);
		put("vmovsldup", _ZMM, _ZMM);


		{
			const char *tbl[] = {
				"valignd",
				"valignq",
			};
			for (size_t i = 0; i < NUM_OF_ARRAY(tbl); i++) {
				const char *name = tbl[i];
				put(name, XMM_KZ, _XMM, _XMM | MEM, IMM);
				put(name, _YMM3, _YMM3, _YMM3 | _MEM, IMM);
				put(name, _ZMM, _ZMM, _ZMM | _MEM, IMM);
			}
		}
		{
			const char tbl[][16] = {
				"vmovhpd",
				"vmovhps",
				"vmovlpd",
				"vmovlps",
			};
			for (size_t i = 0; i < NUM_OF_ARRAY(tbl); i++) {
				put(tbl[i], _XMM3, _XMM3, MEM);
				put(tbl[i], MEM, _XMM3);
			}
		}
#endif
	}
	void put512_X_XM()
	{
		const struct Tbl {
			const char *name;
			bool M_X;
		} tbl[] = {
			{ "vmovddup", false },
			{ "vmovdqa32", true },
			{ "vmovdqa64", true },
			{ "vmovdqu8", true },
			{ "vmovdqu16", true },
			{ "vmovdqu32", true },
			{ "vmovdqu64", true },
			{ "vpabsb", false },
			{ "vpabsw", false },
			{ "vpabsd", false },
			{ "vpabsq", false },
		};
		for (size_t i = 0; i < NUM_OF_ARRAY(tbl); i++) {
			const Tbl& p = tbl[i];
			put(p.name, _XMM|XMM_KZ, _XMM|MEM);
			put(p.name, _YMM|YMM_KZ, _YMM|MEM);
			put(p.name, _ZMM|ZMM_KZ, _ZMM|MEM);
			if (!p.M_X) continue;
			put(p.name, MEM, _XMM);
			put(p.name, MEM, _YMM);
			put(p.name, MEM, _ZMM);
		}
		put("vsqrtpd", XMM_KZ, M_1to2 | _MEM);
		put("vsqrtpd", YMM_KZ, M_1to4 | _MEM);
		put("vsqrtpd", ZMM_KZ, M_1to8 | _MEM);
		put("vsqrtpd", ZMM_KZ, ZMM_ER);

		put("vsqrtps", XMM_KZ, M_1to4 | _MEM);
		put("vsqrtps", YMM_KZ, M_1to8 | _MEM);
		put("vsqrtps", ZMM_KZ, M_1to16 | _MEM);
		put("vsqrtps", ZMM_KZ, ZMM_ER);

		put("vpabsd", ZMM_KZ, M_1to16 | _MEM);
		put("vpabsq", ZMM_KZ, M_1to8 | _MEM);

		put("vbroadcastf32x2", YMM_KZ | ZMM_KZ, _XMM | _MEM);
		put("vbroadcastf32x4", YMM_KZ | ZMM_KZ, _MEM);

		put("vbroadcastf64x2", YMM_KZ | ZMM_KZ, _MEM);
		put("vbroadcastf64x4", ZMM_KZ, _MEM);
	}
	void put512_X_X_XM()
	{
		const struct Tbl {
			const char *name;
			uint64_t mem;
		} tbl[] = {
			{ "vsqrtsd", MEM },
			{ "vsqrtss", MEM },
			{ "vunpckhpd", M_1to2 },
			{ "vunpckhps", M_1to4 },
			{ "vunpcklpd", M_1to2 },
			{ "vunpcklps", M_1to4 },
		};
		for (size_t i = 0; i < NUM_OF_ARRAY(tbl); i++) {
			const Tbl& p = tbl[i];
			put(p.name, XMM_KZ, _XMM, _XMM|p.mem);
		}
	}
	void put512_X3()
	{
#ifdef XBYAK64
		const struct Tbl {
			const char *name;
			uint64_t x1;
			uint64_t x2;
			uint64_t xm;
		} tbl[] = {
			{ "vpacksswb", XMM_KZ, _XMM, _XMM | _MEM },
			{ "vpacksswb", YMM_KZ, _YMM, _YMM | _MEM },
			{ "vpacksswb", ZMM_KZ, _ZMM, _ZMM | _MEM },

			{ "vpackssdw", XMM_KZ, _XMM, _XMM | M_1to4 | _MEM },
			{ "vpackssdw", YMM_KZ, _YMM, _YMM | M_1to8 | _MEM },
			{ "vpackssdw", ZMM_KZ, _ZMM, _ZMM | M_1to16 | _MEM },

			{ "vpackusdw", XMM_KZ, _XMM, _XMM | M_1to4 | _MEM },
			{ "vpackusdw", YMM_KZ, _YMM, _YMM | M_1to8 | _MEM },
			{ "vpackusdw", ZMM_KZ, _ZMM, _ZMM | M_1to16 | _MEM },

			{ "vpackuswb", XMM_KZ, _XMM, _XMM | _MEM },
			{ "vpackuswb", YMM_KZ, _YMM, _YMM | _MEM },
			{ "vpackuswb", ZMM_KZ, _ZMM, _ZMM | _MEM },

			{ "vpaddb", XMM_KZ, _XMM, _XMM | _MEM },
			{ "vpaddw", XMM_KZ, _XMM, _XMM | _MEM },
			{ "vpaddd", XMM_KZ, _XMM, _XMM | M_1to4 | _MEM },
			{ "vpaddq", ZMM_KZ, _ZMM, M_1to8 | _MEM },

			{ "vpaddsb", XMM_KZ, _XMM, _XMM | _MEM },
			{ "vpaddsb", ZMM_KZ, _ZMM, _ZMM | _MEM },

			{ "vpaddsw", XMM_KZ, _XMM, _XMM | _MEM },
			{ "vpaddsw", ZMM_KZ, _ZMM, _ZMM | _MEM },

			{ "vpaddusb", XMM_KZ, _XMM, _XMM | MEM },
			{ "vpaddusb", ZMM_KZ, _ZMM, _ZMM | MEM },

			{ "vpaddusw", XMM_KZ, _XMM, _XMM | MEM },
			{ "vpaddusw", ZMM_KZ, _ZMM, _ZMM | MEM },

			{ "vpsubb", XMM_KZ, _XMM, _XMM | _MEM },
			{ "vpsubw", XMM_KZ, _XMM, _XMM | _MEM },
			{ "vpsubd", XMM_KZ, _XMM, _XMM | M_1to4 | _MEM },
			{ "vpsubq", ZMM_KZ, _ZMM, M_1to8 | _MEM },

			{ "vpsubsb", XMM_KZ, _XMM, _XMM | _MEM },
			{ "vpsubsb", ZMM_KZ, _ZMM, _ZMM | _MEM },

			{ "vpsubsw", XMM_KZ, _XMM, _XMM | _MEM },
			{ "vpsubsw", ZMM_KZ, _ZMM, _ZMM | _MEM },

			{ "vpsubusb", XMM_KZ, _XMM, _XMM | MEM },
			{ "vpsubusb", ZMM_KZ, _ZMM, _ZMM | MEM },

			{ "vpsubusw", XMM_KZ, _XMM, _XMM | MEM },
			{ "vpsubusw", ZMM_KZ, _ZMM, _ZMM | MEM },

			{ "vpandd", ZMM_KZ, _ZMM, _ZMM | M_1to16 | _MEM },
			{ "vpandq", ZMM_KZ, _ZMM, _ZMM | M_1to8 | _MEM },

			{ "vpandnd", ZMM_KZ, _ZMM, _ZMM | M_1to16 | _MEM },
			{ "vpandnq", ZMM_KZ, _ZMM, _ZMM | M_1to8 | _MEM },

			{ "vpavgb", ZMM_KZ, _ZMM, _ZMM },
			{ "vpavgw", ZMM_KZ, _ZMM, _ZMM },

			{ "vpcmpeqb", K2, _ZMM, _ZMM | _MEM },
			{ "vpcmpeqw", K2, _ZMM, _ZMM | _MEM },
			{ "vpcmpeqd", K2, _ZMM, _ZMM | M_1to16 | _MEM },
			{ "vpcmpeqq", K2, _ZMM, _ZMM | M_1to8 | _MEM },

			{ "vpcmpgtb", K2, _ZMM, _ZMM | _MEM },
			{ "vpcmpgtw", K2, _ZMM, _ZMM | _MEM },
			{ "vpcmpgtd", K2, _ZMM, _ZMM | M_1to16 | _MEM },
			{ "vpcmpgtq", K2, _ZMM, _ZMM | M_1to8 | _MEM },

			{ "vpmaddubsw", ZMM_KZ, _ZMM, _ZMM | _MEM },
			{ "vpmaddwd", ZMM_KZ, _ZMM, _ZMM | _MEM },

			{ "vpmaxsb", ZMM_KZ, _ZMM, _ZMM | _MEM },
			{ "vpmaxsw", ZMM_KZ, _ZMM, _ZMM | _MEM },
			{ "vpmaxsd", ZMM_KZ, _ZMM, _ZMM | _MEM | M_1to16 },
			{ "vpmaxsq", ZMM_KZ, _ZMM, _ZMM | _MEM | M_1to8 },

			{ "vpmaxub", ZMM_KZ, _ZMM, _ZMM | _MEM },
			{ "vpmaxuw", ZMM_KZ, _ZMM, _ZMM | _MEM },
			{ "vpmaxud", ZMM_KZ, _ZMM, _ZMM | _MEM | M_1to16 },
			{ "vpmaxuq", ZMM_KZ, _ZMM, _ZMM | _MEM | M_1to8 },

			{ "vpminsb", ZMM_KZ, _ZMM, _ZMM | _MEM },
			{ "vpminsw", ZMM_KZ, _ZMM, _ZMM | _MEM },
			{ "vpminsd", ZMM_KZ, _ZMM, _ZMM | _MEM | M_1to16 },
			{ "vpminsq", ZMM_KZ, _ZMM, _ZMM | _MEM | M_1to8 },

			{ "vpminub", ZMM_KZ, _ZMM, _ZMM | _MEM },
			{ "vpminuw", ZMM_KZ, _ZMM, _ZMM | _MEM },
			{ "vpminud", ZMM_KZ, _ZMM, _ZMM | _MEM | M_1to16 },
			{ "vpminuq", ZMM_KZ, _ZMM, _ZMM | _MEM | M_1to8 },

			{ "vpslldq", _XMM3, _XMM3 | _MEM, IMM8 },
			{ "vpslldq", _YMM3, _YMM3 | _MEM, IMM8 },
			{ "vpslldq", _ZMM, _ZMM | _MEM, IMM8 },

			{ "vpsrldq", _XMM3, _XMM3 | _MEM, IMM8 },
			{ "vpsrldq", _YMM3, _YMM3 | _MEM, IMM8 },
			{ "vpsrldq", _ZMM, _ZMM | _MEM, IMM8 },

			{ "vpsraw", XMM_KZ, _XMM | _MEM, IMM8 },
			{ "vpsraw", ZMM_KZ, _ZMM | _MEM, IMM8 },

			{ "vpsrad", XMM_KZ, _XMM | M_1to4 | _MEM, IMM8 },
			{ "vpsrad", ZMM_KZ, _ZMM | M_1to16 | _MEM, IMM8 },

			{ "vpsraq", XMM, XMM, IMM8 },
			{ "vpsraq", XMM_KZ, _XMM | M_1to2 | _MEM, IMM8 },
			{ "vpsraq", ZMM_KZ, _ZMM | M_1to8 | _MEM, IMM8 },

			{ "vpsllw", _XMM3, _XMM3 | _MEM, IMM8 },
			{ "vpslld", _XMM3, _XMM3 | _MEM | M_1to4, IMM8 },
			{ "vpsllq", _XMM3, _XMM3 | _MEM | M_1to2, IMM8 },

			{ "vpsrlw", XMM_KZ, _XMM | _MEM, IMM8 },
			{ "vpsrlw", ZMM_KZ, _ZMM | _MEM, IMM8 },

			{ "vpsrld", XMM_KZ, _XMM | M_1to4 | _MEM, IMM8 },
			{ "vpsrld", ZMM_KZ, _ZMM | M_1to16 | _MEM, IMM8 },

			{ "vpsrlq", _XMM3, _XMM3 | _MEM | M_1to2, IMM8 },
			{ "vpsrlq", _ZMM, _ZMM | _MEM | M_1to8, IMM8 },

			{ "vpsravw", XMM_KZ | _XMM, _XMM, _XMM | _MEM },
			{ "vpsravw", _ZMM, _ZMM, _MEM },

			{ "vpsravd", XMM_KZ | _XMM, _XMM, _XMM | _MEM },
			{ "vpsravd", _ZMM, _ZMM, M_1to16 | _MEM },

			{ "vpsravq", XMM_KZ | _XMM, _XMM, _XMM | _MEM },
			{ "vpsravq", _ZMM, _ZMM, M_1to8 | _MEM },

			{ "vpsllvw", XMM_KZ | _XMM, _XMM, _XMM | _MEM },
			{ "vpsllvw", _ZMM, _ZMM, _MEM },

			{ "vpsllvd", XMM_KZ | _XMM, _XMM, _XMM | _MEM },
			{ "vpsllvd", _ZMM, _ZMM, M_1to16 | _MEM },

			{ "vpsllvq", XMM_KZ | _XMM, _XMM, _XMM | _MEM },
			{ "vpsllvq", _ZMM, _ZMM, M_1to8 | _MEM },

			{ "vpsrlvw", XMM_KZ | _XMM, _XMM, _XMM | _MEM },
			{ "vpsrlvw", _ZMM, _ZMM, _MEM },

			{ "vpsrlvd", XMM_KZ | _XMM, _XMM, _XMM | _MEM },
			{ "vpsrlvd", _ZMM, _ZMM, M_1to16 | _MEM },

			{ "vpsrlvq", XMM_KZ | _XMM, _XMM, _XMM | _MEM },
			{ "vpsrlvq", _ZMM, _ZMM, M_1to8 | _MEM },

			{ "vpshufb", _XMM | XMM_KZ, _XMM, _XMM | _MEM },
			{ "vpshufb", ZMM_KZ, _ZMM, _MEM },

			{ "vpshufhw", _XMM | XMM_KZ, _XMM | _MEM, IMM8 },
			{ "vpshufhw", ZMM_KZ, _MEM, IMM8 },

			{ "vpshuflw", _XMM | XMM_KZ, _XMM | _MEM, IMM8 },
			{ "vpshuflw", ZMM_KZ, _MEM, IMM8 },

			{ "vpshufd", _XMM | XMM_KZ, _XMM | M_1to4 | _MEM, IMM8 },
			{ "vpshufd", _ZMM | ZMM_KZ, _ZMM | M_1to16 | _MEM, IMM8 },

			{ "vpord", _XMM | XMM_KZ, _XMM, _XMM | M_1to4 | _MEM },
			{ "vpord", _ZMM | ZMM_KZ, _ZMM, M_1to16 | _MEM },

			{ "vporq", _XMM | XMM_KZ, _XMM, _XMM | M_1to2 | _MEM },
			{ "vporq", _ZMM | ZMM_KZ, _ZMM, M_1to8 | _MEM },

			{ "vpxord", _XMM | XMM_KZ, _XMM, _XMM | M_1to4 | _MEM },
			{ "vpxord", _ZMM | ZMM_KZ, _ZMM, M_1to16 | _MEM },

			{ "vpxorq", _XMM | XMM_KZ, _XMM, _XMM | M_1to2 | _MEM },
			{ "vpxorq", _ZMM | ZMM_KZ, _ZMM, M_1to8 | _MEM },

			{ "vpsadbw", _XMM3, _XMM, _XMM | _MEM },
			{ "vpsadbw", _ZMM, _ZMM, _MEM },

			{ "vpmuldq", _XMM3, _XMM, _XMM | M_1to2 | _MEM },
			{ "vpmuldq", ZMM_KZ, _ZMM, M_1to8 | _MEM },

			{ "vpmulhrsw", _XMM3, _XMM, _XMM | _MEM },
			{ "vpmulhrsw", ZMM_KZ, _ZMM, _MEM },

			{ "vpmulhuw", _XMM3, _XMM, _XMM | _MEM },
			{ "vpmulhuw", ZMM_KZ, _ZMM, _MEM },

			{ "vpmulhw", _XMM3, _XMM, _XMM | _MEM },
			{ "vpmulhw", ZMM_KZ, _ZMM, _MEM },

			{ "vpmullw", _XMM3, _XMM, _XMM | _MEM },
			{ "vpmullw", ZMM_KZ, _ZMM, _MEM },

			{ "vpmulld", _XMM3, _XMM, M_1to4 | _MEM },
			{ "vpmulld", ZMM_KZ, _ZMM, M_1to16 | _MEM },

			{ "vpmullq", _XMM3, _XMM, M_1to2 | _MEM },
			{ "vpmullq", ZMM_KZ, _ZMM, M_1to8 | _MEM },

			{ "vpmuludq", _XMM3, _XMM, M_1to2 | _MEM },
			{ "vpmuludq", ZMM_KZ, _ZMM, M_1to8 | _MEM },

			{ "vpunpckhbw", _XMM3, _XMM, _XMM | _MEM },
			{ "vpunpckhbw", _ZMM, _ZMM, _MEM },

			{ "vpunpckhwd", _XMM3, _XMM, _XMM | _MEM },
			{ "vpunpckhwd", _ZMM, _ZMM, _MEM },

			{ "vpunpckhdq", _XMM3, _XMM, M_1to4 | _MEM },
			{ "vpunpckhdq", _ZMM, _ZMM, M_1to16 | _MEM },

			{ "vpunpckhqdq", _XMM3, _XMM, M_1to2 | _MEM },
			{ "vpunpckhqdq", _ZMM, _ZMM, M_1to8 | _MEM },

			{ "vpunpcklbw", _XMM3, _XMM, _XMM | _MEM },
			{ "vpunpcklbw", _ZMM, _ZMM, _MEM },

			{ "vpunpcklwd", _XMM3, _XMM, _XMM | _MEM },
			{ "vpunpcklwd", _ZMM, _ZMM, _MEM },

			{ "vpunpckldq", _XMM3, _XMM, M_1to4 | _MEM },
			{ "vpunpckldq", _ZMM, _ZMM, M_1to16 | _MEM },

			{ "vpunpcklqdq", _XMM3, _XMM, M_1to2 | _MEM },
			{ "vpunpcklqdq", _ZMM, _ZMM, M_1to8 | _MEM },

			{ "vextractf32x4", _XMM | XMM_KZ | _MEM, _YMM | _ZMM, IMM8 },
			{ "vextractf64x2", _XMM | XMM_KZ | _MEM, _YMM | _ZMM, IMM8 },
			{ "vextractf32x8", _YMM | YMM_KZ | _MEM, _ZMM, IMM8 },
			{ "vextractf64x4", _YMM | YMM_KZ | _MEM, _ZMM, IMM8 },

			{ "vextracti32x4", _XMM | XMM_KZ | _MEM, _YMM | _ZMM, IMM8 },
			{ "vextracti64x2", _XMM | XMM_KZ | _MEM, _YMM | _ZMM, IMM8 },
			{ "vextracti32x8", _YMM | YMM_KZ | _MEM, _ZMM, IMM8 },
			{ "vextracti64x4", _YMM | YMM_KZ | _MEM, _ZMM, IMM8 },

			{ "vextractps", REG32 | _MEM, _XMM3, IMM8 },

			{ "vpermb", XMM_KZ, _XMM, _XMM | _MEM },
			{ "vpermb", ZMM_KZ, _ZMM, _ZMM | _MEM },

			{ "vpermw", XMM_KZ, _XMM, _XMM | _MEM },
			{ "vpermw", ZMM_KZ, _ZMM, _ZMM | _MEM },

			{ "vpermd", YMM_KZ, _YMM, _YMM | M_1to8 | _MEM },
			{ "vpermd", ZMM_KZ, _ZMM, _ZMM | M_1to16 | _MEM },

			{ "vpermilpd", XMM_KZ, _XMM, _XMM | M_1to2 | _MEM },
			{ "vpermilpd", ZMM_KZ, _ZMM, M_1to8 | _MEM },
			{ "vpermilpd", XMM_KZ, M_1to2 | _MEM, IMM8 },
			{ "vpermilpd", ZMM_KZ, M_1to8 | _MEM, IMM8 },

			{ "vpermilps", XMM_KZ, _XMM, _XMM | _MEM | M_1to4 },
			{ "vpermilps", ZMM_KZ, _ZMM, _MEM | M_1to16 },
			{ "vpermilps", XMM_KZ, _MEM | M_1to4 | _MEM, IMM8 },
			{ "vpermilps", ZMM_KZ, _MEM | M_1to16 | _MEM, IMM8 },

			{ "vpermpd", YMM_KZ, _YMM | M_1to4 | _MEM, IMM8 },
			{ "vpermpd", ZMM_KZ, _ZMM | M_1to8 | _MEM, IMM8 },
			{ "vpermpd", YMM_KZ, _YMM, M_1to4 | _MEM },
			{ "vpermpd", ZMM_KZ, _ZMM, M_1to8 | _MEM },

			{ "vpermps", YMM_KZ, _YMM, M_1to8 | _MEM },
			{ "vpermps", ZMM_KZ, _ZMM, M_1to16 | _MEM },

			{ "vpermq", YMM_KZ, _YMM | M_1to4 | _MEM, IMM8 },
			{ "vpermq", ZMM_KZ, _ZMM | M_1to8 | _MEM, IMM8 },
			{ "vpermq", YMM_KZ, _YMM, M_1to4 | _MEM },
			{ "vpermq", ZMM_KZ, _ZMM, M_1to8 | _MEM },
		};
		for (size_t i = 0; i < NUM_OF_ARRAY(tbl); i++) {
			const Tbl& p = tbl[i];
			put(p.name, p.x1, p.x2, p.xm);
		}
#endif
	}
	void put512_X3_I()
	{
		const struct Tbl {
			const char *name;
			uint64_t x1;
			uint64_t x2;
			uint64_t xm;
		} tbl[] = {
#ifdef XBYAK64
			{ "vinsertps", _XMM, _XMM, _XMM3 | _MEM },

			{ "vshufpd", XMM_KZ, _XMM, M_1to2 | _MEM },
			{ "vshufpd", ZMM_KZ, _ZMM, M_1to8 | _MEM },

			{ "vshufps", XMM_KZ, _XMM, M_1to4 | _MEM },
			{ "vshufps", ZMM_KZ, _ZMM, M_1to16 | _MEM },

			{ "vinsertf32x4", _YMM | YMM_KZ, _YMM, _XMM | _MEM },
			{ "vinsertf32x4", _ZMM | ZMM_KZ, _ZMM, _XMM | _MEM },

			{ "vinsertf64x2", _YMM | YMM_KZ, _YMM, _XMM | _MEM },
			{ "vinsertf64x2", _ZMM | ZMM_KZ, _ZMM, _XMM | _MEM },

			{ "vinsertf32x8", _ZMM | ZMM_KZ, _ZMM, _YMM | _MEM },
			{ "vinsertf64x4", _ZMM | ZMM_KZ, _ZMM, _YMM | _MEM },

			{ "vinserti32x4", _YMM | YMM_KZ, _YMM, _XMM | _MEM },
			{ "vinserti32x4", _ZMM | ZMM_KZ, _ZMM, _XMM | _MEM },

			{ "vinserti64x2", _YMM | YMM_KZ, _YMM, _XMM | _MEM },
			{ "vinserti64x2", _ZMM | ZMM_KZ, _ZMM, _XMM | _MEM },

			{ "vinserti32x8", _ZMM | ZMM_KZ, _ZMM, _YMM | _MEM },
			{ "vinserti64x4", _ZMM | ZMM_KZ, _ZMM, _YMM | _MEM },
#endif
			{ "vpalignr", ZMM_KZ, _ZMM, _ZMM | _MEM },
		};
		for (size_t i = 0; i < NUM_OF_ARRAY(tbl); i++) {
			const Tbl& p = tbl[i];
			put(p.name, p.x1, p.x2, p.xm, IMM8);
		}
#ifdef XBYAK64
		put("vpextrb", _REG64 | _MEM, _XMM3, IMM8);
		put("vpextrw", _REG64 | _MEM, _XMM3, IMM8);
		put("vpextrd", _REG32 | _MEM, _XMM3, IMM8);
		put("vpextrq", _REG64 | _MEM, _XMM3, IMM8);
		put("vpinsrb", _XMM3, _XMM3, _REG32 | _MEM, IMM8);
		put("vpinsrw", _XMM3, _XMM3, _REG32 | _MEM, IMM8);
		put("vpinsrd", _XMM3, _XMM3, _REG32 | _MEM, IMM8);
		put("vpinsrq", _XMM3, _XMM3, _REG64 | _MEM, IMM8);
#endif
	}
	void put512_FMA()
	{
		const struct Tbl {
			const char *name;
			bool supportYMM;
		} tbl[] = {
			{ "vfmadd", true },
			{ "vfmadd", false },
			{ "vfmaddsub", true },
			{ "vfmsubadd", true },
			{ "vfmsub", true },
			{ "vfmsub", false },
			{ "vfnmadd", true },
			{ "vfnmadd", false },
			{ "vfnmsub", true },
			{ "vfnmsub", false },
		};
		for (size_t i = 0; i < NUM_OF_ARRAY(tbl); i++) {
			const Tbl& p = tbl[i];
			const struct Ord {
				const char *name;
			} ord[] = {
				{ "132" },
				{ "213" },
				{ "231" },
			};
			for (size_t j = 0; j < NUM_OF_ARRAY(ord); j++) {
				const char sufTbl[][2][8] = {
					{ "pd", "ps" },
					{ "sd", "ss" },
				};
				for (size_t k = 0; k < 2; k++) {
					const std::string suf = sufTbl[p.supportYMM ? 0 : 1][k];
					uint64_t mem = 0;
					if (suf == "pd") {
						mem = M_1to2;
					} else if (suf == "ps") {
						mem = M_1to4;
					} else {
						mem = XMM_ER;
					}
					std::string name = std::string(p.name) + ord[j].name + suf;
					const char *q = name.c_str();
					put(q, XMM_KZ, _XMM, mem | _MEM);
					if (!p.supportYMM) continue;
					if (suf == "pd") {
						mem = M_1to8;
					} else if (suf == "ps") {
						mem = M_1to16;
					} else {
						mem = XMM_ER;
					}
					put(q, _ZMM, _ZMM, mem | _MEM);
				}
			}
		}
	}
	void put512_Y_XM()
	{
		const struct Tbl {
			const char *name;
			bool all_xmm; // 2nd param
		} tbl[] = {
			{ "vpmovsxbw", false },
			{ "vpmovsxbd", true },
			{ "vpmovsxbq", true },
			{ "vpmovsxwd", false },
			{ "vpmovsxwq", true },
			{ "vpmovsxdq", false },

			{ "vpmovzxbw", false },
			{ "vpmovzxbd", true },
			{ "vpmovzxbq", true },
			{ "vpmovzxwd", false },
			{ "vpmovzxwq", true },
			{ "vpmovzxdq", false },
		};
		for (size_t i = 0; i < NUM_OF_ARRAY(tbl); i++) {
			const Tbl& p = tbl[i];
			const char *name = p.name;
			put(name, XMM_KZ | YMM, _XMM | _MEM);
			if (p.all_xmm) {
				put(name, ZMM, _XMM | _MEM);
			} else {
				put(name, ZMM, YMM | _MEM);
			}
		}
	}
	void put512_AVX1()
	{
#ifdef XBYAK64
		const struct Tbl {
			std::string name;
			bool only_pd_ps;
		} tbl[] = {
			{ "vadd", false },
			{ "vsub", false },
			{ "vmul", false },
			{ "vdiv", false },
			{ "vmax", false },
			{ "vmin", false },
			{ "vand", true },
			{ "vandn", true },
			{ "vor", true },
			{ "vxor", true },
		};
		for (size_t i = 0; i < NUM_OF_ARRAY(tbl); i++) {
			const struct Suf {
				const char *suf;
				bool supportYMM;
			} sufTbl[] = {
				{ "pd", true },
				{ "ps", true },
				{ "sd", false },
				{ "ss", false },
			};
			for (size_t j = 0; j < NUM_OF_ARRAY(sufTbl); j++) {
				if (tbl[i].only_pd_ps && j == 2) break;
				std::string suf = sufTbl[j].suf;
				std::string name = tbl[i].name + suf;
				const char *p = name.c_str();
				uint64_t mem = 0;
				if (suf == "pd") {
					mem = M_1to2;
				} else if (suf == "ps") {
					mem = M_1to4;
				}
				put(p, _XMM3 | XMM_KZ, _XMM, mem | _MEM);
				if (!sufTbl[j].supportYMM) continue;
				mem = 0;
				if (suf == "pd") {
					mem = M_1to8;
				} else if (suf == "ps") {
					mem = M_1to16;
				}
				put(p, _ZMM, _ZMM, mem | _MEM);
			}
		}
#endif
	}
	void putAVX1()
	{
		const struct Tbl {
			const char *name;
			bool only_pd_ps;
		} tbl[] = {
			{ "add", false },
			{ "sub", false },
			{ "mul", false },
			{ "div", false },
			{ "max", false },
			{ "min", false },
			{ "and", true },
			{ "andn", true },
			{ "or", true },
			{ "xor", true },
		};
		for (size_t i = 0; i < NUM_OF_ARRAY(tbl); i++) {
			const struct Suf {
				const char *suf;
				bool supportYMM;
			} suf[] = {
				{ "pd", true },
				{ "ps", true },
				{ "sd", false },
				{ "ss", false },
			};
			for (size_t j = 0; j < NUM_OF_ARRAY(suf); j++) {
				if (tbl[i].only_pd_ps && j == 2) break;
				std::string name = std::string("v") + tbl[i].name + suf[j].suf;
				const char *p = name.c_str();
				put(p, XMM, XMM | MEM);
				put(p, XMM, XMM, XMM | MEM);
				if (!suf[j].supportYMM) continue;
				put(p, YMM, YMM | MEM);
				put(p, YMM, YMM, YMM | MEM);
				put(p, ZMM, ZMM, ZMM | MEM);
			}
		}
	}
	void put512_cvt()
	{
#ifdef XBYAK64
		put("vcvtdq2pd", XMM_KZ, _XMM | _MEM | M_1to2);
		put("vcvtdq2pd", YMM_KZ, _XMM | _MEM | M_1to4);
		put("vcvtdq2pd", ZMM_KZ, _YMM | _MEM | M_1to8);

		put("vcvtdq2ps", XMM_KZ, _XMM | _MEM | M_1to4);
		put("vcvtdq2ps", YMM_KZ, _YMM | _MEM | M_1to8);
		put("vcvtdq2ps", ZMM_KZ, _ZMM | _MEM | M_1to16);

		put("vcvtpd2dq", _XMM | _XMM3, _XMM | M_xword | M_1to2);
		put("vcvtpd2dq", _XMM | _XMM3, _YMM | M_yword | MY_1to4);
		put("vcvtpd2dq", YMM | YMM_KZ, ZMM | _MEM | M_1to8);
#endif
	}
	void putMin()
	{
#ifdef XBYAK64
		put("vpcmpeqb", K, _XMM3|_YMM, _MEM);
		put("vpcmpeqw", K, _XMM3|_YMM, _MEM);
		put("vpcmpeqd", K, _XMM3|_YMM, _MEM);
		put("vpcmpeqq", K, _XMM3|_YMM, _MEM);
#endif
	}
	void putAVX512()
	{
#ifdef MIN_TEST
		putMin();
#else
		putOpmask();
		separateFunc();
		putCombi();
		separateFunc();
		putCmpK();
		separateFunc();
		putBroadcast();
		separateFunc();
		putAVX512_M_X();
		separateFunc();
		put_vmov();
		separateFunc();
		put512_X_XM();
		separateFunc();
		put512_X_X_XM();
		separateFunc();
		put512_X3();
		separateFunc();
		put512_X3_I();
		separateFunc();
		put512_FMA();
		separateFunc();
		put512_Y_XM();
		separateFunc();
		put512_AVX1();
		separateFunc();
		put512_cvt();
		separateFunc();
		putMisc1();
#endif
	}
};

int main(int argc, char *[])
{
	Test test(argc > 1);
	test.put();
}
