#include "compile.h"
#include "util.h"

#include "ext/vec.h"
#define SV_IMPLEMENTATION
#include "ext/sv.h"

#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

typedef vec_t(chaw_token_t) chaw_vec_token_t;

String_View chaw_punctuators[] = {
	[CHAW_PUNCT_BRACKET_OPEN] = SV_STATIC("["),
	[CHAW_PUNCT_BRACKET_CLOSE] = SV_STATIC("]"),
	[CHAW_PUNCT_PARENT_OPEN] = SV_STATIC("("),
	[CHAW_PUNCT_PARENT_CLOSE] = SV_STATIC(")"),
	[CHAW_PUNCT_BRACE_OPEN] = SV_STATIC("{"),
	[CHAW_PUNCT_BRACE_CLOSE] = SV_STATIC("}"),
	[CHAW_PUNCT_DOT] = SV_STATIC("."),
	[CHAW_PUNCT_COMMA] = SV_STATIC(","),

	[CHAW_PUNCT_NAMESPACE] = SV_STATIC("::"),

	[CHAW_PUNCT_INCREMENT] = SV_STATIC("++"),
	[CHAW_PUNCT_DECREMENT] = SV_STATIC("--"),

	[CHAW_PUNCT_ASSIGN_ADD] = SV_STATIC("+="),
	[CHAW_PUNCT_ASSIGN_SUB] = SV_STATIC("-="),
	[CHAW_PUNCT_ASSIGN_MUL] = SV_STATIC("*="),
	[CHAW_PUNCT_ASSIGN_DIV] = SV_STATIC("/="),
	[CHAW_PUNCT_ASSIGN_MOD] = SV_STATIC("%="),

	[CHAW_PUNCT_ASSIGN_NEG] = SV_STATIC("~="),
	[CHAW_PUNCT_ASSIGN_OR] = SV_STATIC("|="),
	[CHAW_PUNCT_ASSIGN_XOR] = SV_STATIC("^="),
	[CHAW_PUNCT_ASSIGN_AND] = SV_STATIC("&="),
	[CHAW_PUNCT_ASSIGN_SHR] = SV_STATIC(">>="),
	[CHAW_PUNCT_ASSIGN_SHL] = SV_STATIC("<<="),

	[CHAW_PUNCT_EQUAL] = SV_STATIC("=="),
	[CHAW_PUNCT_NOT_EQUAL] = SV_STATIC("!="),
	[CHAW_PUNCT_LESS_OR_EQ_THAN] = SV_STATIC("<="),
	[CHAW_PUNCT_GREATER_OR_EQ_THAN] = SV_STATIC(">="),
	[CHAW_PUNCT_LESS_THAN] = SV_STATIC("<"),
	[CHAW_PUNCT_GREATER_THAN] = SV_STATIC(">"),
	[CHAW_PUNCT_SET] = SV_STATIC("="),
	[CHAW_PUNCT_ADD] = SV_STATIC("+"),
	[CHAW_PUNCT_SUB] = SV_STATIC("-"),
	[CHAW_PUNCT_MUL] = SV_STATIC("*"),
	[CHAW_PUNCT_DIV] = SV_STATIC("/"),
	[CHAW_PUNCT_MOD] = SV_STATIC("%"),
	[CHAW_PUNCT_NOT] = SV_STATIC("!"),

	[CHAW_PUNCT_NEG] = SV_STATIC("~"),
	[CHAW_PUNCT_OR] = SV_STATIC("|"),
	[CHAW_PUNCT_XOR] = SV_STATIC("^"),
	[CHAW_PUNCT_AND] = SV_STATIC("&"),

	[CHAW_PUNCT_THEN] = SV_STATIC("?"),
	[CHAW_PUNCT_ELSE] = SV_STATIC(":"),
};

String_View chaw_script_punctuators[] = {
	[CHAW_SCRIPT_PUNCT_BRACKET_OPEN] = SV_STATIC("["),
	[CHAW_SCRIPT_PUNCT_BRACKET_CLOSE] = SV_STATIC("]"),
	[CHAW_SCRIPT_PUNCT_PARENT_OPEN] = SV_STATIC("("),
	[CHAW_SCRIPT_PUNCT_PARENT_CLOSE] = SV_STATIC(")"),
	[CHAW_SCRIPT_PUNCT_BRACE_OPEN] = SV_STATIC("{"),
	[CHAW_SCRIPT_PUNCT_BRACE_CLOSE] = SV_STATIC("}"),
	[CHAW_SCRIPT_PUNCT_DOT] = SV_STATIC("."),
	[CHAW_SCRIPT_PUNCT_COMMA] = SV_STATIC(","),

	[CHAW_SCRIPT_PUNCT_SEMICOLON] = SV_STATIC(";"),

	[CHAW_SCRIPT_PUNCT_NAMESPACE] = SV_STATIC("::"),

	[CHAW_SCRIPT_PUNCT_INCREMENT] = SV_STATIC("++"),
	[CHAW_SCRIPT_PUNCT_DECREMENT] = SV_STATIC("--"),

	[CHAW_SCRIPT_PUNCT_LOOP_DECLARE] = SV_STATIC("@"),
	[CHAW_SCRIPT_PUNCT_LOOP_REF] = SV_STATIC("$"),

	[CHAW_SCRIPT_PUNCT_ASSIGN_ADD] = SV_STATIC("+="),
	[CHAW_SCRIPT_PUNCT_ASSIGN_SUB] = SV_STATIC("-="),
	[CHAW_SCRIPT_PUNCT_ASSIGN_MUL] = SV_STATIC("*="),
	[CHAW_SCRIPT_PUNCT_ASSIGN_DIV] = SV_STATIC("/="),
	[CHAW_SCRIPT_PUNCT_ASSIGN_MOD] = SV_STATIC("%="),

	[CHAW_SCRIPT_PUNCT_ASSIGN_NEG] = SV_STATIC("~="),
	[CHAW_SCRIPT_PUNCT_ASSIGN_OR] = SV_STATIC("|="),
	[CHAW_SCRIPT_PUNCT_ASSIGN_XOR] = SV_STATIC("^="),
	[CHAW_SCRIPT_PUNCT_ASSIGN_AND] = SV_STATIC("&="),
	[CHAW_SCRIPT_PUNCT_ASSIGN_SHR] = SV_STATIC(">>="),
	[CHAW_SCRIPT_PUNCT_ASSIGN_SHL] = SV_STATIC("<<="),

	[CHAW_SCRIPT_PUNCT_EQUAL] = SV_STATIC("=="),
	[CHAW_SCRIPT_PUNCT_NOT_EQUAL] = SV_STATIC("!="),
	[CHAW_SCRIPT_PUNCT_LESS_OR_EQ_THAN] = SV_STATIC("<="),
	[CHAW_SCRIPT_PUNCT_GREATER_OR_EQ_THAN] = SV_STATIC(">="),
	[CHAW_SCRIPT_PUNCT_LESS_THAN] = SV_STATIC("<"),
	[CHAW_SCRIPT_PUNCT_GREATER_THAN] = SV_STATIC(">"),
	[CHAW_SCRIPT_PUNCT_SET] = SV_STATIC("="),
	[CHAW_SCRIPT_PUNCT_ADD] = SV_STATIC("+"),
	[CHAW_SCRIPT_PUNCT_SUB] = SV_STATIC("-"),
	[CHAW_SCRIPT_PUNCT_MUL] = SV_STATIC("*"),
	[CHAW_SCRIPT_PUNCT_DIV] = SV_STATIC("/"),
	[CHAW_SCRIPT_PUNCT_MOD] = SV_STATIC("%"),
	[CHAW_SCRIPT_PUNCT_NOT] = SV_STATIC("!"),

	[CHAW_SCRIPT_PUNCT_NEG] = SV_STATIC("~"),
	[CHAW_SCRIPT_PUNCT_OR] = SV_STATIC("|"),
	[CHAW_SCRIPT_PUNCT_XOR] = SV_STATIC("^"),
	[CHAW_SCRIPT_PUNCT_AND] = SV_STATIC("&"),
	
	[CHAW_SCRIPT_PUNCT_THEN] = SV_STATIC("?"),
	[CHAW_SCRIPT_PUNCT_ELSE] = SV_STATIC(":"),
};

String_View chaw_keywords[] = {
	[CHAW_KWORD_ITEM] = SV_STATIC("ITEM"),
	[CHAW_KWORD_FREE] = SV_STATIC("FREE"),
	[CHAW_KWORD_VAR] = SV_STATIC("VAR"),
	[CHAW_KWORD_BYTE] = SV_STATIC("BYTE"),
	[CHAW_KWORD_SHORT] = SV_STATIC("SHORT"),
	[CHAW_KWORD_LONG] = SV_STATIC("LONG"),
	[CHAW_KWORD_CHUNK] = SV_STATIC("CHUNK"),
	[CHAW_KWORD_ENDCHUNK] = SV_STATIC("ENDCHUNK"),
	[CHAW_KWORD_ADOPT] = SV_STATIC("ADOPT"),
	[CHAW_KWORD_CHILD] = SV_STATIC("CHILD"),
	[CHAW_KWORD_PARENT] = SV_STATIC("PARENT"),
	[CHAW_KWORD_BO] = SV_STATIC("BO"),
	[CHAW_KWORD_OSK] = SV_STATIC("OSK"),
	[CHAW_KWORD_STN] = SV_STATIC("STN"),
	[CHAW_KWORD_STZ] = SV_STATIC("STZ"),
	[CHAW_KWORD_SZ] = SV_STATIC("SZ"),
	[CHAW_KWORD_ST] = SV_STATIC("ST"),
	[CHAW_KWORD_ALIGN] = SV_STATIC("ALIGN"),
	[CHAW_KWORD_FILE] = SV_STATIC("FILE"),
	[CHAW_KWORD_PACKEDFILE] = SV_STATIC("PACKEDFILE"),
	[CHAW_KWORD_META] = SV_STATIC("META"),
	[CHAW_KWORD_BITMAP] = SV_STATIC("BITMAP"),
	[CHAW_KWORD_MASK] = SV_STATIC("MASK"),
	[CHAW_KWORD_MIDI] = SV_STATIC("MIDI"),
	[CHAW_KWORD_SCRIPT] = SV_STATIC("SCRIPT"),
	[CHAW_KWORD_SCRIPTPF] = SV_STATIC("SCRIPTPF"),
	[CHAW_KWORD_GL] = SV_STATIC("GL"),
	[CHAW_KWORD_AL] = SV_STATIC("AL"),
	[CHAW_KWORD_GG] = SV_STATIC("GG"),
	[CHAW_KWORD_AG] = SV_STATIC("AG"),
	[CHAW_KWORD_GST] = SV_STATIC("GST"),
	[CHAW_KWORD_AST] = SV_STATIC("AST"),
	[CHAW_KWORD_MACBO] = SV_STATIC("MACBO"),
	[CHAW_KWORD_WINBO] = SV_STATIC("WINBO"),
	[CHAW_KWORD_MACOSK] = SV_STATIC("MACOSK"),
	[CHAW_KWORD_WINOSK] = SV_STATIC("WINOSK"),
	[CHAW_KWORD_LONER] = SV_STATIC("LONER"),
	[CHAW_KWORD_CURSOR] = SV_STATIC("CURSOR"),
	[CHAW_KWORD_PALETTE] = SV_STATIC("PALETTE"),
	[CHAW_KWORD_PREPACKED] = SV_STATIC("PREPACKED"),
	[CHAW_KWORD_PACK] = SV_STATIC("PACK"),
	[CHAW_KWORD_PACKFMT] = SV_STATIC("PACKFMT"),
	[CHAW_KWORD_SUBFILE] = SV_STATIC("SUBFILE"),
	[CHAW_KWORD_SET] = SV_STATIC("SET"),
};

String_View chaw_script_keywords[] = {
	[CHAW_SCRIPT_KWORD_IF] = SV_STATIC("If"),
	[CHAW_SCRIPT_KWORD_ELIF] = SV_STATIC("ElIf"),
	[CHAW_SCRIPT_KWORD_ElSE] = SV_STATIC("Else"),
	[CHAW_SCRIPT_KWORD_END] = SV_STATIC("End"),
	[CHAW_SCRIPT_KWORD_WHILE] = SV_STATIC("While"),
	[CHAW_SCRIPT_KWORD_BREAK] = SV_STATIC("Break"),
	[CHAW_SCRIPT_KWORD_CONTINUE] = SV_STATIC("Continue"),
};

chaw_punct_type_t chaw_find_matching_punctuator(char *s) {
	for (size_t i = 0; i < CHAW_COUNTOF(chaw_punctuators); i++) {
		String_View *sv = &chaw_punctuators[i];
		if (sv->count == 0) {
			continue;
		}
		if (strncmp(s, sv->data, sv->count) == 0) {
			return i;
		}
	}
	return CHAW_PUNCT_UNKNOWN;
}

chaw_kword_type_t chaw_find_matching_keyword(char *s) {
	for (size_t i = 1; i < CHAW_COUNTOF(chaw_keywords); i++) {
		String_View *sv = &chaw_keywords[i];
		if (sv->count == 0) {
			continue;
		}
		if (chaw_strncasecmp(s, sv->data, sv->count) == 0) {
			return i;
		}
	}
	return CHAW_KWORD_UNKNOWN;
}

chaw_script_punct_type_t chaw_script_find_matching_punctuator(char *s) {
	for (size_t i = 0; i < CHAW_COUNTOF(chaw_script_punctuators); i++) {
		String_View *sv = &chaw_script_punctuators[i];
		if (sv->count == 0) {
			continue;
		}
		if (strncmp(s, sv->data, sv->count) == 0) {
			return i;
		}
	}
	return CHAW_SCRIPT_PUNCT_UNKNOWN;
}

chaw_script_kword_type_t chaw_script_find_matching_keyword(char *s) {
	for (size_t i = 0; i < CHAW_COUNTOF(chaw_script_keywords); i++) {
		String_View *sv = &chaw_script_keywords[i];
		if (sv->count == 0) {
			continue;
		}
		if (chaw_strncasecmp(s, sv->data, sv->count) == 0) {
			return i;
		}
	}
	return CHAW_SCRIPT_KWORD_UNKNOWN;
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

bool chaw_script_is_valid_punctuator(char *s) {
	return chaw_script_find_matching_punctuator(s) != CHAW_SCRIPT_PUNCT_UNKNOWN;
}

bool chaw_script_is_valid_keyword(char *s) {
	return chaw_script_find_matching_keyword(s) != CHAW_SCRIPT_KWORD_UNKNOWN;
}

void chaw_tokenize_punctuator(char **data, chaw_token_t *token) {
	token->type = CHAW_TOKEN_PUNCTUATOR;
	token->value.punct = CHAW_PUNCT_UNKNOWN;

	char *literal = *data;
	token->value.punct = chaw_find_matching_punctuator(literal);
	literal += chaw_punctuators[token->value.punct].count;

	*data = literal;
}

void chaw_tokenize_keyword(char **data, chaw_token_t *token) {
	token->type = CHAW_TOKEN_KEYWORD;
	token->value.kword = CHAW_KWORD_UNKNOWN;

	char *literal = *data;
	token->value.kword = chaw_find_matching_keyword(literal);
	literal += chaw_keywords[token->value.kword].count;

	*data = literal;
}

void chaw_script_tokenize_punctuator(char **data, chaw_token_t *token) {
	token->type = CHAW_TOKEN_PUNCTUATOR;
	token->value.script.punct = CHAW_SCRIPT_PUNCT_UNKNOWN;

	char *literal = *data;
	token->value.script.punct = chaw_script_find_matching_punctuator(literal);
	literal += chaw_script_punctuators[token->value.punct].count;

	*data = literal;
}

void chaw_script_tokenize_keyword(char **data, chaw_token_t *token) {
	token->type = CHAW_TOKEN_KEYWORD;
	token->value.script.kword = CHAW_SCRIPT_KWORD_UNKNOWN;

	char *literal = *data;
	token->value.script.kword = chaw_script_find_matching_keyword(literal);
	literal += chaw_script_keywords[token->value.kword].count;

	*data = literal;
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

int chaw_tokenize_preprocessor(chaw_vec_token_t *vec, char **data_pointer) {
	chaw_token_t current_token;

	// NOTE(bjrkk): for now, we skip anything preprocessor related.
	// chomp doesn't actually do any preprocessing, leaving it all to
	// MSVC to do all the preprocessing magic.
	char *data = *data_pointer;
	while (*data != '\0') {
		if (*data == '\n') {
			break;
		}

		if (isspace(*data)) {
			data++;
			continue;
		}

		if (*data == '\\') {
			for (; *data != '\0' && *data != '\n'; data++);
			continue;
		}

		data++;
	}
	
	*data_pointer = data + 1;
	return 0;
}

int chaw_tokenize_script(chaw_vec_token_t *vec, char **data_pointer) {
	chaw_token_t current_token;

	char *data = *data_pointer;
	while (*data != '\0') {
		if (isspace(*data)) {
			data++;
			continue;
		}

		String_View *endchunk_identifier = &chaw_keywords[CHAW_KWORD_ENDCHUNK];
		if (chaw_strncasecmp(endchunk_identifier->data, data, endchunk_identifier->count) == 0) {
			break;
		} 
		
		if (isdigit(*data)) {
			chaw_tokenize_number(&data, &current_token);
		} else if (*data == '\"') {
			chaw_tokenize_string(&data, &current_token);
		} else if (*data == '\'') {
			chaw_tokenize_tag(&data, &current_token);
		} else if (chaw_script_is_valid_keyword(data)) {
			chaw_script_tokenize_keyword(&data, &current_token);
		} else if (chaw_is_valid_identifier_char(*data)) {
			chaw_tokenize_identifier(&data, &current_token);
		} else if (chaw_script_is_valid_punctuator(data)) {
			chaw_script_tokenize_punctuator(&data, &current_token);
		} else {
			printf("chaw: error: unexpected script token\n");
			return 1;
		}

		vec_push(vec, current_token);
	}
	*data_pointer = data;
	return 0;
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

		if (current_token.type == CHAW_TOKEN_KEYWORD) {
			if (current_token.value.kword == CHAW_KWORD_SCRIPT || current_token.value.kword == CHAW_KWORD_SCRIPTPF) {
				current_token.type = CHAW_TOKEN_SCRIPT_BEGIN;
				vec_push(vec, current_token);

				int result = chaw_tokenize_script(vec, &data);
				if (result != 0) {
					return result;
				}
				
				current_token.type = CHAW_TOKEN_SCRIPT_END;
				vec_push(vec, current_token);
				continue;
			}
		}

		if (*data == '#') {
			current_token.type = CHAW_TOKEN_PREPROCESSOR_BEGIN;
			vec_push(vec, current_token);

			int result = chaw_tokenize_preprocessor(vec, &data);
			if (result != 0) {
				return result;
			}
			
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

size_t chaw_script_dump_tokens(chaw_vec_token_t *vec, size_t begin) {
	size_t i = begin;
	for (; i < vec->length; i++) {
		chaw_token_t *token = &vec->data[i];
		if (token->type == CHAW_TOKEN_SCRIPT_END) {
			break;
		}
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
			printf("{ punctuator: '%s' }\n", chaw_script_punctuators[token->value.script.punct].data);
			break;
		case CHAW_TOKEN_KEYWORD:
			printf("{ keywords: '%s' }\n", chaw_script_keywords[token->value.script.kword].data);
			break;
		default:
			printf("{ unknown }\n");
			break;
		}
	}
	return i - begin;
}

size_t chaw_preprocessor_dump_tokens(chaw_vec_token_t *vec, size_t begin) {
	size_t i = begin;
	for (; i < vec->length; i++) {
		chaw_token_t *token = &vec->data[i];
		if (token->type == CHAW_TOKEN_PREPROCESSOR_END) {
			break;
		}
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
			printf("{ punctuator: '%s' }\n", chaw_punctuators[token->value.punct].data);
			break;
		case CHAW_TOKEN_KEYWORD:
			printf("{ keywords: '%s' }\n", chaw_keywords[token->value.kword].data);
			break;
		case CHAW_TOKEN_TAG:
			printf("{ tag: '%#010x' }\n", token->value.tag);
			break;
		default:
			printf("{ unknown }\n");
			break;
		}
	}
	return i - begin;
}

size_t chaw_dump_tokens(chaw_vec_token_t *vec, size_t begin) {
	size_t i = begin;
	for (; i < vec->length; i++) {
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
			printf("{ punctuator: '%s' }\n", chaw_punctuators[token->value.punct].data);
			break;
		case CHAW_TOKEN_KEYWORD:
			printf("{ keywords: '%s' }\n", chaw_keywords[token->value.kword].data);
			break;
		case CHAW_TOKEN_TAG:
			printf("{ tag: '%#010x' }\n", token->value.tag);
			break;
		case CHAW_TOKEN_SCRIPT_BEGIN:
			printf("{ script: {\n");
			i++;
			i += chaw_script_dump_tokens(vec, i);
			printf("} }\n");
			break;
		case CHAW_TOKEN_PREPROCESSOR_BEGIN:
			printf("{ preprocessor: {\n");
			i += chaw_preprocessor_dump_tokens(vec, i);
			printf("} }\n");
			break;
		default:
			printf("{ unknown }\n");
			break;
		}
	}
	return i - begin;
}

void chaw_compile_cht(char *data) {
	chaw_vec_token_t tokens;
	vec_init(&tokens);
	if (chaw_tokenize(&tokens, data) != 0) {
		printf("chaw: error: failed to generate tokens\n");
	} else {
		printf("chaw: success: generated %d tokens\n", tokens.length);
	}
	chaw_dump_tokens(&tokens, 0);
	vec_deinit(&tokens);
}