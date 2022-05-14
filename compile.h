#pragma once
#include <stdint.h>

typedef enum {
	CHAW_TOKEN_INTEGER,
	CHAW_TOKEN_DECIMAL,
	CHAW_TOKEN_STRING,
	CHAW_TOKEN_IDENTIFER,
	CHAW_TOKEN_PUNCTUATOR,
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

typedef struct {
	chaw_token_type_t type;
	union {
		int64_t integer;
		double decimal;
		char const *string;
		chaw_punct_type_t punct;
	} value;
} chaw_token_t;

void chaw_compile_cht(char *data);