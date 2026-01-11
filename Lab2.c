#include "Lab2.h"


char* get_processed_string(char* buffer) {
	string_ result = { 0 };
	char* src = buffer;
	ErrorType error = NONE;

	if (!buffer) {
		return NULL;
	}

	// Прикидываем размер с запасом - если все цифры станут "EIGHT"
	size_t initial_capacity = strlen(buffer) * 5 + 1;
	if (initial_capacity < STR_START_CAPACITY_) {
		initial_capacity = STR_START_CAPACITY_;
	}

	result.capacity = initial_capacity;
	result.buffer = (char*)calloc(result.capacity, sizeof(char));
	if (!result.buffer) {
		error = MAIN_BUFFER_ALLOC_FAILED;
		goto clear;
	}

	while (*src != '\0') {
		if ((unsigned char)*src >= 0x80) {
			src++;
			continue;
		}

		if (isdigit(*src)) {
			int digit = *src - '0';
			const char* replacement = NULL;
			char temp_buffer[16];
			size_t repl_len = 0;

			// Четные -> слова
			if (digit % 2 == 0) {
				const char* even_words[] = { "ZERO", "TWO", "FOUR", "SIX", "EIGHT" };
				replacement = even_words[digit / 2];
				repl_len = strlen(replacement);
			}
			// Нечетные -> коды типа #2, #6, #10...
			else {
				if (digit >= 5) {
					temp_buffer[0] = '#';
					temp_buffer[1] = '1';
					switch (digit) {
					case 5: temp_buffer[2] = '0'; break;
					case 7: temp_buffer[2] = '4'; break;
					case 9: temp_buffer[2] = '8'; break;
					}
					temp_buffer[3] = '\0';
				}
				else {
					// 1->2, 3->6
					temp_buffer[0] = '#';
					switch (digit) {
					case 1: temp_buffer[1] = '2'; break;
					case 3: temp_buffer[1] = '6'; break;
					}
					temp_buffer[2] = '\0';
				}
				replacement = temp_buffer;
				repl_len = strlen(replacement);
			}

			// Проверяем хватит ли места
			if (result.len + repl_len + 1 >= result.capacity) {
				size_t new_capacity = result.capacity + STR_CAPACITY_STEP_;
				// случай когда STR_CAPACITY_STEP_ мало
				while (result.len + repl_len + 1 >= new_capacity) {
					new_capacity += STR_CAPACITY_STEP_;
				}

				char* t_buffer = (char*)realloc(result.buffer, new_capacity);
				if (!t_buffer) {
					error = BUFFER_REALLOC_FAILED;
					goto clear;
				}
				result.buffer = t_buffer;
				result.capacity = new_capacity;
			}

			// Копируем замену
			memcpy(result.buffer + result.len, replacement, repl_len);
			result.len += repl_len;
			result.buffer[result.len] = '\0';

			src++;
		}
		else {
			// Обычные символы просто переносим
			if (result.len + 2 >= result.capacity) {
				char* t_buffer = (char*)realloc(result.buffer, result.capacity + STR_CAPACITY_STEP_);
				if (!t_buffer) {
					error = BUFFER_REALLOC_FAILED;
					goto clear;
				}
				result.capacity += STR_CAPACITY_STEP_;
				result.buffer = t_buffer;
			}

			result.buffer[result.len++] = *src++;
			result.buffer[result.len] = '\0';
		}
	}

clear:
	if (error != NONE) {
		free(result.buffer);
		return NULL;
	}

	return result.buffer;
}


// TASK 1

// Читает строку по символу, каждый раз выделяя новый буфер
char* read_string_dynamic_malloc(FILE* istream) {
	ErrorType error = NONE;
	size_t str_len = 0;
	char* buffer = NULL;
	int ch = 0;

	if (!istream) {
		return NULL;
	}

	// Начинаем с пустой строки
	buffer = (char*)malloc(str_len + 1);
	if (!buffer) {
		return NULL;
	}
	buffer[str_len] = '\0';

	while ((ch = getc(istream)) != '\n' && ch != EOF) {
		size_t new_str_len = str_len + 2; // +1 символ +1 для '\0'

		// Нельзя realloc по заданию, только malloc + free
		char* t_buffer = (char*)malloc(new_str_len);
		if (!t_buffer) {
			error = TEMP_BUFFER_ALLOC_FAILED;
			goto clear;
		}

		// Копируем старое содержимое
		for (size_t i = 0; i < str_len; i++) {
			t_buffer[i] = buffer[i];
		}

		t_buffer[str_len++] = (char)ch;
		t_buffer[str_len] = '\0';

		free(buffer);
		buffer = t_buffer;
	}

clear:
	if (error != NONE) {
		free(buffer);
		return NULL;
	}

	return buffer;
}


char* read_string_dynamic_realloc(FILE* istream) {
	string_ str = { 0 };
	str.capacity = STR_START_CAPACITY_;
	int ch = 0;
	ErrorType error = NONE;

	if (!istream) {
		return NULL;
	}

	str.buffer = (char*)calloc(str.capacity, sizeof(char));
	if (!str.buffer) {
		error = MAIN_BUFFER_ALLOC_FAILED;
		goto clear;
	}

	while ((ch = getc(istream)) != '\n' && ch != EOF) {
		// Если не влезает - расширяем
		if (str.len + 2 >= str.capacity) {
			char* t_buffer = (char*)realloc(str.buffer, str.capacity + STR_CAPACITY_STEP_);
			if (!t_buffer) {
				error = BUFFER_REALLOC_FAILED;
				goto clear;
			}
			str.capacity += STR_CAPACITY_STEP_;
			str.buffer = t_buffer;
		}

		str.buffer[str.len++] = (char)ch;
		str.buffer[str.len] = '\0';
	}

clear:
	if (error != NONE) {
		free(str.buffer);
		return NULL;
	}

	return str.buffer;
}


 int read_and_process_string_to_file(ReaderType type,char* out_filepath) {
	FILE* temp_file = NULL, * out_file = NULL;
	char* string = NULL, * processed_str = NULL;
	int error = NONE;

	// Читаем строку выбранным методом
	switch (type) {
	case MALLOC:
		string = read_string_dynamic_malloc(stdin);
		break;
	case REALLOC:
	default:
		string = read_string_dynamic_realloc(stdin);
		break;
	}

	if (!string) {
		error = GETTING_STRING_FAILED;
		goto clear;
	}

	// Пишем во временный файл
	temp_file = fopen(TEMP_FILE_NAME_, "w");
	if (!temp_file) {
		error = FILE_OPEN_FAILED;
		goto clear;
	}

	if (fputs(string, temp_file) == EOF) {
		error = FILE_WRITE_FAILURE;
		goto clear;
	}

	fclose(temp_file);
	temp_file = NULL;
	free(string);
	string = NULL;

	// Читаем обратно из файла
	temp_file = fopen(TEMP_FILE_NAME_, "r");
	if (!temp_file) {
		error = FILE_OPEN_FAILED;
		goto clear;
	}

	switch (type) {
	case MALLOC:
		string = read_string_dynamic_malloc(temp_file);
		break;
	case REALLOC:
		string = read_string_dynamic_realloc(temp_file);
		break;
	}

	if (!string) {
		error = FILE_READ_FAILURE;
		goto clear;
	}

	// Обрабатываем строку
	processed_str = get_processed_string(string);
	if (!processed_str) {
		error = STRING_PROCESSING_FAILED;
		goto clear;
	}

	free(string);
	string = NULL;

	// Записываем результат в выходной файл
	out_file = fopen(out_filepath, "w");
	if (!out_file) {
		error = FILE_OPEN_FAILED;
		goto clear;
	}

	if (fputs(processed_str, out_file) == EOF) {
		error = FILE_WRITE_FAILURE;
		goto clear;
	}

clear:
	if (string) {
		free(string);
	}
	if (processed_str) {
		free(processed_str);
	}
	if (temp_file) {
		fclose(temp_file);
	}
	if (out_file) {
		fclose(out_file);
	}

	return error;
}

