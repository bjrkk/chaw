#pragma once
#include <stdint.h>

typedef enum {
	CHAW_TOKEN_INTEGER,
	CHAW_TOKEN_DECIMAL,
	CHAW_TOKEN_STRING,
	CHAW_TOKEN_TAG,
	CHAW_TOKEN_IDENTIFER,
	CHAW_TOKEN_PUNCTUATOR,
	CHAW_TOKEN_KEYWORD,
	CHAW_TOKEN_PREPROCESSOR_BEGIN,
	CHAW_TOKEN_PREPROCESSOR_END,
} chaw_token_type_t;

typedef enum {
	CHAW_PUNCT_UNKNOWN,
	CHAW_PUNCT_BRACKET_OPEN,
	CHAW_PUNCT_BRACKET_CLOSE,
	CHAW_PUNCT_PARENT_OPEN,
	CHAW_PUNCT_PARENT_CLOSE,
	CHAW_PUNCT_BRACE_OPEN,
	CHAW_PUNCT_BRACE_CLOSE,
	CHAW_PUNCT_DOT,
	CHAW_PUNCT_COMMA,
	CHAW_PUNCT_SEMICOLON,

	CHAW_PUNCT_LOOP_DECLARE,
	CHAW_PUNCT_LOOP_IDENTIFIER,

	CHAW_PUNCT_NAMESPACE,

	/* Operators */
	CHAW_PUNCT_INCREMENT,
	CHAW_PUNCT_DECREMENT,
	CHAW_PUNCT_ASSIGN_ADD,
	CHAW_PUNCT_ASSIGN_SUB,
	CHAW_PUNCT_ASSIGN_MUL,
	CHAW_PUNCT_ASSIGN_DIV,
	CHAW_PUNCT_ASSIGN_MOD,
	CHAW_PUNCT_ASSIGN_NEG,
	CHAW_PUNCT_ASSIGN_OR,
	CHAW_PUNCT_ASSIGN_XOR,
	CHAW_PUNCT_ASSIGN_AND,
	CHAW_PUNCT_ASSIGN_SHR,
	CHAW_PUNCT_ASSIGN_SHL,

	CHAW_PUNCT_EQUAL,
	CHAW_PUNCT_NOT_EQUAL,
	CHAW_PUNCT_LESS_OR_EQ_THAN,
	CHAW_PUNCT_GREATER_OR_EQ_THAN,
	CHAW_PUNCT_LESS_THAN,
	CHAW_PUNCT_GREATER_THAN,

	CHAW_PUNCT_SET,
	CHAW_PUNCT_ADD,
	CHAW_PUNCT_SUB,
	CHAW_PUNCT_MUL,
	CHAW_PUNCT_DIV,
	CHAW_PUNCT_MOD,
	CHAW_PUNCT_NOT,
	CHAW_PUNCT_NEG,
	CHAW_PUNCT_OR,
	CHAW_PUNCT_XOR,
	CHAW_PUNCT_AND,

	CHAW_PUNCT_THEN,
	CHAW_PUNCT_ELSE,
} chaw_punct_type_t;

typedef enum {
	CHAW_KWORD_UNKNOWN,
	CHAW_KWORD_ITEM,
	CHAW_KWORD_FREE,
	CHAW_KWORD_VAR,
	CHAW_KWORD_BYTE,
	CHAW_KWORD_SHORT,
	CHAW_KWORD_LONG,
	CHAW_KWORD_CHUNK,
	CHAW_KWORD_ENDCHUNK,
	CHAW_KWORD_ADOPT,
	CHAW_KWORD_CHILD,
	CHAW_KWORD_PARENT,
	CHAW_KWORD_BO,
	CHAW_KWORD_OSK,
	CHAW_KWORD_STN,
	CHAW_KWORD_STZ,
	CHAW_KWORD_SZ,
	CHAW_KWORD_ST,
	CHAW_KWORD_ALIGN,
	CHAW_KWORD_FILE,
	CHAW_KWORD_PACKEDFILE,
	CHAW_KWORD_META,
	CHAW_KWORD_BITMAP,
	CHAW_KWORD_MASK,
	CHAW_KWORD_MIDI,
	CHAW_KWORD_SCRIPT,
	CHAW_KWORD_SCRIPTPF,
	CHAW_KWORD_GL,
	CHAW_KWORD_AL,
	CHAW_KWORD_GG,
	CHAW_KWORD_AG,
	CHAW_KWORD_GST,
	CHAW_KWORD_AST,
	CHAW_KWORD_MACBO,
	CHAW_KWORD_WINBO,
	CHAW_KWORD_MACOSK,
	CHAW_KWORD_WINOSK,
	CHAW_KWORD_LONER,
	CHAW_KWORD_CURSOR,
	CHAW_KWORD_PALETTE,
	CHAW_KWORD_PREPACKED,
	CHAW_KWORD_PACK,
	CHAW_KWORD_PACKFMT,
	CHAW_KWORD_SUBFILE,
	CHAW_KWORD_SET
} chaw_kword_type_t;

typedef struct {
	chaw_token_type_t type;
	union {
		int64_t integer;
		double decimal;
		uint32_t tag;
		char const *string;
		chaw_punct_type_t punct;
		chaw_kword_type_t kword;
	} value;
} chaw_token_t;

void chaw_compile_cht(char *data);