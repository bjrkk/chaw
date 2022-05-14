#include "compile.h"
#include "util.h"
#include "ext/vec.h"

#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

typedef vec_t(chaw_token_t) chaw_vec_token_t;

typedef struct {
	size_t length;
	char const *string;
} chaw_punct_metadata_t;

typedef struct {
	size_t length;
	char const *string;
} chaw_kword_metadata_t;

#define DECLARE_PUNCT(t, s) [t] = { \
	.length = CHAW_COUNTOF(s), \
	.string = s \
}

chaw_punct_metadata_t chaw_punctuators[] = {
	DECLARE_PUNCT(CHAW_PUNCT_BRACKET_OPEN, "["),
	DECLARE_PUNCT(CHAW_PUNCT_BRACKET_CLOSE, "]"),
	DECLARE_PUNCT(CHAW_PUNCT_PARENT_OPEN, "("),
	DECLARE_PUNCT(CHAW_PUNCT_PARENT_CLOSE, ")"),
	DECLARE_PUNCT(CHAW_PUNCT_BRACE_OPEN, "{"),
	DECLARE_PUNCT(CHAW_PUNCT_BRACE_CLOSE, "}"),
	DECLARE_PUNCT(CHAW_PUNCT_DOT, "."),
	DECLARE_PUNCT(CHAW_PUNCT_COMMA, ","),
	DECLARE_PUNCT(CHAW_PUNCT_SEMICOLON, ";"),

	DECLARE_PUNCT(CHAW_PUNCT_LOOP_DECLARE, "@"),
	DECLARE_PUNCT(CHAW_PUNCT_LOOP_IDENTIFIER, "$"),

	DECLARE_PUNCT(CHAW_PUNCT_NAMESPACE, "::"),

	DECLARE_PUNCT(CHAW_PUNCT_INCREMENT, "++"),
	DECLARE_PUNCT(CHAW_PUNCT_DECREMENT, "--"),

	DECLARE_PUNCT(CHAW_PUNCT_ASSIGN_ADD, "+="),
	DECLARE_PUNCT(CHAW_PUNCT_ASSIGN_SUB, "-="),
	DECLARE_PUNCT(CHAW_PUNCT_ASSIGN_MUL, "*="),
	DECLARE_PUNCT(CHAW_PUNCT_ASSIGN_DIV, "/="),
	DECLARE_PUNCT(CHAW_PUNCT_ASSIGN_MOD, "%="),

	DECLARE_PUNCT(CHAW_PUNCT_ASSIGN_NEG, "~="),
	DECLARE_PUNCT(CHAW_PUNCT_ASSIGN_OR, "|="),
	DECLARE_PUNCT(CHAW_PUNCT_ASSIGN_XOR, "^="),
	DECLARE_PUNCT(CHAW_PUNCT_ASSIGN_AND, "&="),
	DECLARE_PUNCT(CHAW_PUNCT_ASSIGN_SHR, ">>="),
	DECLARE_PUNCT(CHAW_PUNCT_ASSIGN_SHL, "<<="),

	DECLARE_PUNCT(CHAW_PUNCT_EQUAL, "=="),
	DECLARE_PUNCT(CHAW_PUNCT_NOT_EQUAL, "!="),
	DECLARE_PUNCT(CHAW_PUNCT_LESS_OR_EQ_THAN, "<="),
	DECLARE_PUNCT(CHAW_PUNCT_GREATER_OR_EQ_THAN, ">="),
	DECLARE_PUNCT(CHAW_PUNCT_LESS_THAN, "<"),
	DECLARE_PUNCT(CHAW_PUNCT_GREATER_THAN, ">"),

	DECLARE_PUNCT(CHAW_PUNCT_SET, "="),

	DECLARE_PUNCT(CHAW_PUNCT_ADD, "+"),
	DECLARE_PUNCT(CHAW_PUNCT_SUB, "-"),
	DECLARE_PUNCT(CHAW_PUNCT_MUL, "*"),
	DECLARE_PUNCT(CHAW_PUNCT_DIV, "/"),
	DECLARE_PUNCT(CHAW_PUNCT_MOD, "%"),

	DECLARE_PUNCT(CHAW_PUNCT_NOT, "!"),
	DECLARE_PUNCT(CHAW_PUNCT_NEG, "~"),
	DECLARE_PUNCT(CHAW_PUNCT_OR, "|"),
	DECLARE_PUNCT(CHAW_PUNCT_XOR, "^"),
	DECLARE_PUNCT(CHAW_PUNCT_AND, "&"),

	DECLARE_PUNCT(CHAW_PUNCT_THEN, "?"),
	DECLARE_PUNCT(CHAW_PUNCT_ELSE, ":"),
};

#undef DECLARE_PUNCT

#define DECLARE_KWORD(t, s) [t] = { \
	.length = CHAW_COUNTOF(s), \
	.string = s \
}

chaw_kword_metadata_t chaw_keywords[] = {
	DECLARE_KWORD(CHAW_KWORD_ITEM, "ITEM"),
	DECLARE_KWORD(CHAW_KWORD_FREE, "FREE"),
	DECLARE_KWORD(CHAW_KWORD_VAR, "VAR"),
	DECLARE_KWORD(CHAW_KWORD_BYTE, "BYTE"),
	DECLARE_KWORD(CHAW_KWORD_SHORT, "SHORT"),
	DECLARE_KWORD(CHAW_KWORD_LONG, "LONG"),
	DECLARE_KWORD(CHAW_KWORD_CHUNK, "CHUNK"),
	DECLARE_KWORD(CHAW_KWORD_ENDCHUNK, "ENDCHUNK"),
	DECLARE_KWORD(CHAW_KWORD_ADOPT, "ADOPT"),
	DECLARE_KWORD(CHAW_KWORD_CHILD, "CHILD"),
	DECLARE_KWORD(CHAW_KWORD_PARENT, "PARENT"),
	DECLARE_KWORD(CHAW_KWORD_BO, "BO"),
	DECLARE_KWORD(CHAW_KWORD_OSK, "OSK"),
	DECLARE_KWORD(CHAW_KWORD_STN, "STN"),
	DECLARE_KWORD(CHAW_KWORD_STZ, "STZ"),
	DECLARE_KWORD(CHAW_KWORD_SZ, "SZ"),
	DECLARE_KWORD(CHAW_KWORD_ST, "ST"),
	DECLARE_KWORD(CHAW_KWORD_ALIGN, "ALIGN"),
	DECLARE_KWORD(CHAW_KWORD_FILE, "FILE"),
	DECLARE_KWORD(CHAW_KWORD_PACKEDFILE, "PACKEDFILE"),
	DECLARE_KWORD(CHAW_KWORD_META, "META"),
	DECLARE_KWORD(CHAW_KWORD_BITMAP, "BITMAP"),
	DECLARE_KWORD(CHAW_KWORD_MASK, "MASK"),
	DECLARE_KWORD(CHAW_KWORD_MIDI, "MIDI"),
	DECLARE_KWORD(CHAW_KWORD_SCRIPT, "SCRIPT"),
	DECLARE_KWORD(CHAW_KWORD_SCRIPTPF, "SCRIPTPF"),
	DECLARE_KWORD(CHAW_KWORD_GL, "GL"),
	DECLARE_KWORD(CHAW_KWORD_AL, "AL"),
	DECLARE_KWORD(CHAW_KWORD_GG, "GG"),
	DECLARE_KWORD(CHAW_KWORD_AG, "AG"),
	DECLARE_KWORD(CHAW_KWORD_GST, "GST"),
	DECLARE_KWORD(CHAW_KWORD_AST, "AST"),
	DECLARE_KWORD(CHAW_KWORD_MACBO, "MACBO"),
	DECLARE_KWORD(CHAW_KWORD_WINBO, "WINBO"),
	DECLARE_KWORD(CHAW_KWORD_MACOSK, "MACOSK"),
	DECLARE_KWORD(CHAW_KWORD_WINOSK, "WINOSK"),
	DECLARE_KWORD(CHAW_KWORD_LONER, "LONER"),
	DECLARE_KWORD(CHAW_KWORD_CURSOR, "CURSOR"),
	DECLARE_KWORD(CHAW_KWORD_PALETTE, "PALETTE"),
	DECLARE_KWORD(CHAW_KWORD_PREPACKED, "PREPACKED"),
	DECLARE_KWORD(CHAW_KWORD_PACK, "PACK"),
	DECLARE_KWORD(CHAW_KWORD_PACKFMT, "PACKFMT"),
	DECLARE_KWORD(CHAW_KWORD_SUBFILE, "SUBFILE"),
	DECLARE_KWORD(CHAW_KWORD_SET, "SET"),
};

#undef DECLARE_KWORD

chaw_punct_type_t chaw_find_matching_punctuator(char *s) {
	for (size_t i = 0; i < CHAW_COUNTOF(chaw_punctuators); i++) {
		chaw_punct_metadata_t *metadata = &chaw_punctuators[i];
		if (metadata->length == 0) {
			continue;
		}
		if (strncmp(s, metadata->string, metadata->length - 1) == 0) {
			return i;
		}
	}
	return CHAW_PUNCT_UNKNOWN;
}

chaw_kword_type_t chaw_find_matching_keyword(char *s) {
	for (size_t i = 0; i < CHAW_COUNTOF(chaw_keywords); i++) {
		chaw_kword_metadata_t *metadata = &chaw_keywords[i];
		if (metadata->length == 0) {
			continue;
		}
		if (chaw_strncasecmp(s, metadata->string, metadata->length - 1) == 0) {
			return i;
		}
	}
	return CHAW_KWORD_UNKNOWN;
}

bool chaw_is_valid_identifier_char(char c) {
	if (c >= 'a' && c <= 'z') {
		return true;
	} else if (c >= 'A' && c <= 'Z') {
		return true;
	} else if (c >= '0' && c <= '9') {
		return true;
	} else if (c == '_') {
		return true;
	} else {
		return false;
	}
}

bool chaw_is_valid_punctuator(char *s) {
	return chaw_find_matching_punctuator(s) != CHAW_PUNCT_UNKNOWN;
}

bool chaw_is_valid_keyword(char *s) {
	return chaw_find_matching_keyword(s) != CHAW_KWORD_UNKNOWN;
}

void chaw_tokenize_punctuator(char **data, chaw_token_t *token) {
	token->type = CHAW_TOKEN_PUNCTUATOR;
	token->value.punct = CHAW_PUNCT_UNKNOWN;

	char *literal = *data;
	token->value.punct = chaw_find_matching_punctuator(literal);
	literal += chaw_punctuators[token->value.punct].length - 2;

	*data = literal + 1;
}

void chaw_tokenize_keyword(char **data, chaw_token_t *token) {
	token->type = CHAW_TOKEN_KEYWORD;
	token->value.kword = CHAW_KWORD_UNKNOWN;

	char *literal = *data;
	token->value.kword = chaw_find_matching_keyword(literal);
	literal += chaw_keywords[token->value.kword].length - 2;

	*data = literal + 1;
}

void chaw_tokenize_identifier(char **data, chaw_token_t *token) {
	token->type = CHAW_TOKEN_IDENTIFER;

	char *literal = *data;

	char *begin = literal;
	char *end = literal;

	for (; *end != '\0'; end++) {
		if (!chaw_is_valid_identifier_char(*end)) {
			break;
		}
	}
	
	size_t len = end - begin;

	char *str = (char*)malloc(len + 1);
	memcpy(str, begin, len);
	str[len] = 0;

	token->value.string = str;

	*data = end;
}

void chaw_tokenize_string(char **data, chaw_token_t *token) {
	token->type = CHAW_TOKEN_STRING;

	char *literal = *data;
	char delimiter = *literal;
	literal++;

	char *begin = literal;
	char *end = literal;

	for (; *end != '\0' && *end != delimiter; end++) {
		if (end[0] == '\\' && end[1] == delimiter) {
			end++;
		}
	}
	
	size_t len = end - begin;

	char *str = (char*)malloc(len + 1);
	for (size_t i = 0; i < len; i++) {
		if (begin[i] == '\\') {
			switch (begin[i + 1]) {
			case 'r':
				str[i] = '\r';
				break;
			case 'n':
				str[i] = '\n';
				break;
			case 't':
				str[i] = '\t';
				break;
			case 'b':
				str[i] = '\b';
				break;
			case '0':
				str[i] = '\0';
				break;
			default:
				str[i] = begin[i + 1];
				break;
			}
		} else {
			str[i] = begin[i];
		}
	}
	str[len] = 0;

	token->value.string = str;

	*data = end + 1;
}

void chaw_tokenize_tag(char **data, chaw_token_t *token) {
	token->type = CHAW_TOKEN_TAG;

	char *literal = *data;
	char delimiter = *literal;
	
	literal++;
	uint32_t tag = 0;

	size_t i = 0;
	for (; i < sizeof(token->value.tag); i++) {
		tag <<= 8;
		tag |= literal[i];
		if (literal[i] == delimiter) {
			break;
		}
	}
	
	// TODO(bjrkk): should make this an error instead of a warning
	// like in chomp. plus, we need some way of telling the tokenizer
	// that we screwed up.
	if (literal[i] != delimiter) {
		printf("chaw: warning: tag larger than 4 characters\n");
	}

	literal += i;
	for (; *literal != '\0' && *literal != delimiter; literal++);

	token->value.tag = tag;

	*data = literal + 1;
}

void chaw_tokenize_number(char **data, chaw_token_t *token) {
	token->type = CHAW_TOKEN_INTEGER;
	token->value.integer = 0;

	int radix = 10;

	char *literal = *data;
	if (strncmp(literal, "0x", 2) == 0) {
		literal += 2;
		radix = 16;
	} else if (strncmp(literal, "0o", 2) == 0) {
		literal += 2;
		radix = 8;
	} else if (strncmp(literal, "0b", 2) == 0) {
		literal += 2;
		radix = 2;
	}

	for (; *literal != '\0'; literal++) {
		if (*literal >= '0' && *literal <= '9') {
			token->value.integer *= radix;
			token->value.integer += (*literal - '0');
		} else if (*literal >= 'A' && *literal <= 'Z' && radix > 10) {
			token->value.integer *= radix;
			token->value.integer += (*literal - 'A') + 10;
		} else if (*literal >= 'a' && *literal <= 'z' && radix > 10) {
			token->value.integer *= radix;
			token->value.integer += (*literal - 'a') + 10;
		} else {
			break;
		}
	}

	if (*literal == '.' && radix == 10) {
		token->type = CHAW_TOKEN_DECIMAL;
		token->value.decimal = token->value.integer;
		literal++;
		float depth = radix;
		for (; *literal != '\0' && isdigit(*literal); literal++) {
			token->value.decimal += (*literal - '0') / depth;
			depth *= radix;
		}
		if (*literal == 'f') {
			literal++;
		}
	}

	*data = literal;
}

int chaw_tokenize_preprocessor(chaw_vec_token_t *vec, char **data) {
	chaw_token_t current_token;
	char *prep = *data;
	for (; *prep != '\0'; prep++) {
		if (*prep == '\\') {
			for (; *prep != '\0' && *prep != '\n'; prep++);
			continue;
		}
		if (*prep == '\n') {
			break;
		}
		if (isspace(*prep)) {
			continue;
		}
	}
	*data = prep + 1;
	return 0;
}

void chaw_get_line_and_column_from_index(char *s, size_t j, size_t *ln, size_t *col) {
	(*ln) = 0;
	(*col) = 0;

	for (size_t i = 0; i < j; i++) {
		char c = s[i];
		switch (c) {
		case '\r':
			(*col) = 0;
			break;
		case '\n':
			(*ln)++;
			(*col) = 0;
			break;
		default:
			if (isprint(c)) {
				(*col)++;
			}
			break;
		}
	}
}

int chaw_tokenize(chaw_vec_token_t *vec, char *data) {
	chaw_token_t current_token;
	char *begin = data;
	while (*data != '\0') {
		if (isspace(*data)) {
			data++;
			continue;
		}

		if (strncmp(data, "//", 2) == 0) {
			for (; *data != '\0'; data++) {
				if (*data == '\n') {
					data++;
					break;
				}
			}
			continue;
		}

		if (strncmp(data, "/*", 2) == 0) {
			for (; *data != '\0'; data++) {
				if (strncmp(data, "*/", 2) == 0) {
					data += 2;
					break;
				}
			}
			continue;
		}

		if (*data == '#') {
			current_token.type = CHAW_TOKEN_PREPROCESSOR_BEGIN;
			int result = chaw_tokenize_preprocessor(vec, &data) != 0;
			if (result != 0) {
				return result;
			}
			vec_push(vec, current_token);
			
			current_token.type = CHAW_TOKEN_PREPROCESSOR_END;
			vec_push(vec, current_token);
			continue;
		} else if (isdigit(*data)) {
			chaw_tokenize_number(&data, &current_token);
		} else if (*data == '\"') {
			chaw_tokenize_string(&data, &current_token);
		} else if (*data == '\'') {
			chaw_tokenize_tag(&data, &current_token);
		} else if (chaw_is_valid_keyword(data)) {
			chaw_tokenize_keyword(&data, &current_token);
		} else if (chaw_is_valid_identifier_char(*data)) {
			chaw_tokenize_identifier(&data, &current_token);
		} else if (chaw_is_valid_punctuator(data)) {
			chaw_tokenize_punctuator(&data, &current_token);
		} else {
			size_t index = data - begin;
			size_t ln, col;
			chaw_get_line_and_column_from_index(begin, index, &ln ,&col);
			printf("chaw: error: (%zu, %zu:%zu) unexpected token\n", index, ln + 1, col + 1);
			return 1;
		}

		vec_push(vec, current_token);
	}

	return 0;
}

void chaw_dump_tokens(chaw_vec_token_t *vec) {
	for (size_t i = 0; i < vec->length; i++) {
		chaw_token_t *token = &vec->data[i];
		switch (token->type) {
		case CHAW_TOKEN_INTEGER:
			printf("{ number: %lld }\n", token->value.integer);
			break;
		case CHAW_TOKEN_DECIMAL:
			printf("{ number: %f }\n", token->value.decimal);
			break;
		case CHAW_TOKEN_STRING:
			printf("{ string: '%s' }\n", token->value.string);
			break;
		case CHAW_TOKEN_IDENTIFER:
			printf("{ identifier: '%s' }\n", token->value.string);
			break;
		case CHAW_TOKEN_PUNCTUATOR:
			printf("{ punctuator: '%s' }\n", chaw_punctuators[token->value.punct].string);
			break;
		case CHAW_TOKEN_KEYWORD:
			printf("{ keyword: '%s' }\n", chaw_keywords[token->value.kword].string);
			break;
		case CHAW_TOKEN_TAG:
			printf("{ tag: '%#010x' }\n", token->value.tag);
			break;
		default:
			printf("{ unknown }\n");
			break;
		}
	}
}

void chaw_compile_cht(char *data) {
	chaw_vec_token_t tokens;
	vec_init(&tokens);
	if (chaw_tokenize(&tokens, data) != 0) {
		printf("chaw: error: failed to generate tokens\n");
	} else {
		printf("chaw: success: generated %d tokens\n", tokens.length);
	}
	chaw_dump_tokens(&tokens);
	vec_deinit(&tokens);
}