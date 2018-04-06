/************************************************************
	Project#1:	CLP & DDL
 ************************************************************/

#include "db.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys/stat.h>
#include <sys/types.h>

#if defined(_WIN32) || defined(_WIN64)
  #define strcasecmp _stricmp
#endif

int main(int argc, char** argv)
{
	int rc = 0;
	token_list *tok_list=NULL, *tok_ptr=NULL, *tmp_tok_ptr=NULL;

	if ((argc != 2) || (strlen(argv[1]) == 0))
	{
		printf("Usage: db \"command statement\"");
		return 1;
	}

	rc = initialize_tpd_list();

  if (rc)
  {
		printf("\nError in initialize_tpd_list().\nrc = %d\n", rc);
  }
	else
	{
    rc = get_token(argv[1], &tok_list);

		/* Test code */
		tok_ptr = tok_list;
    /* Print out token by token */
		while (tok_ptr != NULL)
		{
			// printf("%16s \t%d \t %d\n",tok_ptr->tok_string, tok_ptr->tok_class,
			// 	      tok_ptr->tok_value);
			tok_ptr = tok_ptr->next;
		}
    /* If rc is equal to 0 -> do semantic */
		if (!rc)
		{
			rc = do_semantic(tok_list);
		}
    /* Else print error statement */
		if (rc)
		{
			tok_ptr = tok_list;
			while (tok_ptr != NULL)
			{
				if ((tok_ptr->tok_class == error) ||
					  (tok_ptr->tok_value == INVALID))
				{
					printf("Error in the string: %s\n", tok_ptr->tok_string);
					printf("rc=%d\n", rc);
					break;
				}
				tok_ptr = tok_ptr->next;
			}
		}

    /* Whether the token list is valid or not, we need to free the memory */
		tok_ptr = tok_list;
		while (tok_ptr != NULL)
		{
      tmp_tok_ptr = tok_ptr->next;
      free(tok_ptr);
      tok_ptr=tmp_tok_ptr;
		}
	}

	return rc;
}

/*************************************************************
	This is a lexical analyzer for simple SQL statements
 *************************************************************/
int get_token(char* command, token_list** tok_list)
{
  printf("%s\n", command);
	int rc=0,i,j;
	char *start, *cur, temp_string[MAX_TOK_LEN];
	bool done = false;

	start = cur = command;
	while (!done)
	{
		bool found_keyword = false;

		/* This is the TOP Level for each token */
		/* Copies the character '\0' to the first MAX_TOK_LEN characters of the string pointed to, by the argument str. */
	  memset ((void*)temp_string, '\0', MAX_TOK_LEN);
		i = 0;

		/* Get rid of all the leading blanks */
		while (*cur == ' ')
			cur++;

		if (cur && isalpha(*cur))
		{
			// find valid identifier
			int t_class;
			do
			{
				temp_string[i++] = *cur++;
			}
			while ((isalnum(*cur)) || (*cur == '_'));

			if (!(strchr(STRING_BREAK, *cur)))
			{
				/* If the next char following the keyword or identifier
				   is not a blank, (, ), or a comma, then append this
					 character to temp_string, and flag this as an error */
				temp_string[i++] = *cur++;
				add_to_list(tok_list, temp_string, error, INVALID);
				rc = INVALID;
				done = true;
			}
			else
			{

				// We have an identifier with at least 1 character
				// Now check if this ident is a keyword
				for (j = 0, found_keyword = false; j < TOTAL_KEYWORDS_PLUS_TYPE_NAMES; j++)
				{
					if ((strcasecmp(keyword_table[j], temp_string) == 0))
					{
						found_keyword = true;
						break;
					}
				}

				if (found_keyword)
				{
				  if (KEYWORD_OFFSET+j < K_CREATE)
						t_class = type_name;
					else if (KEYWORD_OFFSET+j >= F_SUM)
            t_class = function_name;
          else
					  t_class = keyword;

					add_to_list(tok_list, temp_string, t_class, KEYWORD_OFFSET+j);
				}
				else
				{
					if (strlen(temp_string) <= MAX_IDENT_LEN)
					  add_to_list(tok_list, temp_string, identifier, IDENT);
					else
					{
						add_to_list(tok_list, temp_string, error, INVALID);
						rc = INVALID;
						done = true;
					}
				}

				if (!*cur)
				{
					add_to_list(tok_list, "", terminator, EOC);
					done = true;
				}
			}
		}
		else if (isdigit(*cur))
		{
			// find valid number
			do
			{
				temp_string[i++] = *cur++;
			}
			while (isdigit(*cur));

			if (!(strchr(NUMBER_BREAK, *cur)))
			{
				/* If the next char following the keyword or identifier
				   is not a blank or a ), then append this
					 character to temp_string, and flag this as an error */
				temp_string[i++] = *cur++;
				add_to_list(tok_list, temp_string, error, INVALID);
				rc = INVALID;
				done = true;
			}
			else
			{
				add_to_list(tok_list, temp_string, constant, INT_LITERAL);

				if (!*cur)
				{
					add_to_list(tok_list, "", terminator, EOC);
					done = true;
				}
			}
		}
		else if ((*cur == '(') || (*cur == ')') || (*cur == ',') || (*cur == '*')
		         || (*cur == '=') || (*cur == '<') || (*cur == '>'))
		{
			/* Catch all the symbols here. Note: no look ahead here. */
			int t_value;
			switch (*cur)
			{
				case '(' : t_value = S_LEFT_PAREN; break;
				case ')' : t_value = S_RIGHT_PAREN; break;
				case ',' : t_value = S_COMMA; break;
				case '*' : t_value = S_STAR; break;
				case '=' : t_value = S_EQUAL; break;
				case '<' : t_value = S_LESS; break;
				case '>' : t_value = S_GREATER; break;
			}

			temp_string[i++] = *cur++;

			add_to_list(tok_list, temp_string, symbol, t_value);

			if (!*cur)
			{
				add_to_list(tok_list, "", terminator, EOC);
				done = true;
			}
		}
    else if (*cur == '\'')
    {
      /* Find STRING_LITERRAL */
			int t_class;
      cur++;
			do
			{
				temp_string[i++] = *cur++;
			}
			while ((*cur) && (*cur != '\''));

      temp_string[i] = '\0';

			if (!*cur)
			{
				/* If we reach the end of line */
				add_to_list(tok_list, temp_string, error, INVALID);
				rc = INVALID;
				done = true;
			}
      else /* must be a ' */
      {
        add_to_list(tok_list, temp_string, constant, STRING_LITERAL);
        cur++;
				if (!*cur)
				{
					add_to_list(tok_list, "", terminator, EOC);
					done = true;
        }
      }
    }
		else
		{
			if (!*cur)
			{
				add_to_list(tok_list, "", terminator, EOC);
				done = true;
			}
			else
			{
				/* not a ident, number, or valid symbol */
				temp_string[i++] = *cur++;
				add_to_list(tok_list, temp_string, error, INVALID);
				rc = INVALID;
				done = true;
			}
		}
	}

  return rc;
}

void add_to_list(token_list **tok_list, char *tmp, int t_class, int t_value)
{
	token_list *cur = *tok_list;
	token_list *ptr = NULL;

	// printf("%16s \t%d \t %d\n",tmp, t_class, t_value);

	ptr = (token_list*)calloc(1, sizeof(token_list));
	strcpy(ptr->tok_string, tmp);
	ptr->tok_class = t_class;
	ptr->tok_value = t_value;
	ptr->next = NULL;

  if (cur == NULL)
		*tok_list = ptr;
	else
	{
		while (cur->next != NULL)
			cur = cur->next;

		cur->next = ptr;
	}
	return;
}

int do_semantic(token_list *tok_list)
{
	int rc = 0, cur_cmd = INVALID_STATEMENT;
	bool unique = false;

  /* Set current pointer to token list */
  token_list *cur = tok_list;
  /* If it's CREATE and next pointer is not null & the token value of next value is table */
	if ((cur->tok_value == K_CREATE) &&
			((cur->next != NULL) && (cur->next->tok_value == K_TABLE)))
	{
		printf("CREATE TABLE statement\n");
		cur_cmd = CREATE_TABLE;
    /* Skip the next 2 address space */
		cur = cur->next->next;
	}
  /* Else if it's DROP */
	else if ((cur->tok_value == K_DROP) &&
					((cur->next != NULL) && (cur->next->tok_value == K_TABLE)))
	{
		printf("DROP TABLE statement\n");
		cur_cmd = DROP_TABLE;
		cur = cur->next->next;
	}
  /* Else if it's LIST */
	else if ((cur->tok_value == K_LIST) &&
					((cur->next != NULL) && (cur->next->tok_value == K_TABLE)))
	{
		printf("LIST TABLE statement\n");
		cur_cmd = LIST_TABLE;
		cur = cur->next->next;
	}
	else if ((cur->tok_value == K_LIST) &&
					((cur->next != NULL) && (cur->next->tok_value == K_SCHEMA)))
	{
		printf("LIST SCHEMA statement\n");
		cur_cmd = LIST_SCHEMA;
		cur = cur->next->next;
	}
  else if ((cur->tok_value == K_INSERT) &&
          ((cur->next != NULL) && (cur->next->tok_value == K_INTO)))
  {
    printf("INSERT statement\n");
    cur_cmd = INSERT;
    cur = cur->next->next;
  }
  else if ((cur->tok_value == K_SELECT) &&
        ((cur->next != NULL) && (cur->next->tok_value == S_STAR)))
  {
    printf("SELECT statement\n");
    cur_cmd = SELECT;
    cur = cur->next->next;
  }
	else
  {
		printf("Invalid statement\n");
		rc = cur_cmd;
	}

	if (cur_cmd != INVALID_STATEMENT)
	{
		switch(cur_cmd)
		{
			case CREATE_TABLE:
						rc = sem_create_table(cur);
						break;
			case DROP_TABLE:
						rc = sem_drop_table(cur);
						break;
			case LIST_TABLE:
						rc = sem_list_tables();
						break;
			case LIST_SCHEMA:
						rc = sem_list_schema(cur);
						break;
      case INSERT:
            rc = sem_insert_record(cur);
            break;
      case SELECT:
            rc = sem_select_all(cur);
            break;
			default:
					; /* no action */
		}
	}
	return rc;
}

int sem_drop_table(token_list *t_list)
{
	int rc = 0;
	token_list *cur;
	tpd_entry *tab_entry = NULL;

	cur = t_list;
	if ((cur->tok_class != keyword) &&
		  (cur->tok_class != identifier) &&
			(cur->tok_class != type_name))
	{
		// Error
		rc = INVALID_TABLE_NAME;
		cur->tok_value = INVALID;
	}
	else
	{
		if (cur->next->tok_value != EOC)
		{
			rc = INVALID_STATEMENT;
			cur->next->tok_value = INVALID;
		}
		else
		{
			if ((tab_entry = get_tpd_from_list(cur->tok_string)) == NULL)
			{
				rc = TABLE_NOT_EXIST;
				cur->tok_value = INVALID;
			}
			else
			{
				/* Found a valid tpd, drop it from tpd list */
				rc = drop_tpd_from_list(cur->tok_string);
        char filename[MAX_IDENT_LEN+5];
        strcpy(filename, cur->tok_string);
        strcat(filename, ".tab");
        remove(filename);
        printf("DELETED Filename: %s\n", filename);
			}
		}
	}

  return rc;
}

int sem_list_tables()
{
	int rc = 0;
	int num_tables = g_tpd_list->num_tables;
	tpd_entry *cur = &(g_tpd_list->tpd_start);

	if (num_tables == 0)
	{
		printf("\nThere are currently no tables defined\n");
	}
	else
	{
		printf("\nTable List\n");
		printf("*****************\n");
		while (num_tables-- > 0)
		{
			printf("%s\n", cur->table_name);
			if (num_tables > 0)
			{
				cur = (tpd_entry*)((char*)cur + cur->tpd_size);
			}
		}
		printf("****** End ******\n");
	}

  return rc;
}

int sem_list_schema(token_list *t_list)
{
	int rc = 0;
	token_list *cur;
	tpd_entry *tab_entry = NULL;
	cd_entry  *col_entry = NULL;
	char tab_name[MAX_IDENT_LEN+1];
	char filename[MAX_IDENT_LEN+1];
	bool report = false;
	FILE *fhandle = NULL;
	int i = 0;

	cur = t_list;

	if (cur->tok_value != K_FOR)
  {
		rc = INVALID_STATEMENT;
		cur->tok_value = INVALID;
	}
	else
	{
		cur = cur->next;

		if ((cur->tok_class != keyword) &&
			  (cur->tok_class != identifier) &&
				(cur->tok_class != type_name))
		{
			// Error
			rc = INVALID_TABLE_NAME;
			cur->tok_value = INVALID;
		}
		else
		{
			memset(filename, '\0', MAX_IDENT_LEN+1);
			strcpy(tab_name, cur->tok_string);
			cur = cur->next;

			if (cur->tok_value != EOC)
			{
				if (cur->tok_value == K_TO)
				{
					cur = cur->next;

					if ((cur->tok_class != keyword) &&
						  (cur->tok_class != identifier) &&
							(cur->tok_class != type_name))
					{
						// Error
						rc = INVALID_REPORT_FILE_NAME;
						cur->tok_value = INVALID;
					}
					else
					{
						if (cur->next->tok_value != EOC)
						{
							rc = INVALID_STATEMENT;
							cur->next->tok_value = INVALID;
						}
						else
						{
							/* We have a valid file name */
							strcpy(filename, cur->tok_string);
							report = true;
						}
					}
				}
				else
				{
					/* Missing the TO keyword */
					rc = INVALID_STATEMENT;
					cur->tok_value = INVALID;
				}
			}

			if (!rc)
			{
				if ((tab_entry = get_tpd_from_list(tab_name)) == NULL)
				{
					rc = TABLE_NOT_EXIST;
					cur->tok_value = INVALID;
				}
				else
				{
					if (report)
					{
						if((fhandle = fopen(filename, "a+tc")) == NULL)
						{
							rc = FILE_OPEN_ERROR;
						}
					}

					if (!rc)
					{
						/* Find correct tpd, need to parse column and index information */

						/* First, write the tpd_entry information */
						printf("Table PD size            (tpd_size)    = %d\n", tab_entry->tpd_size);
						printf("Table Name               (table_name)  = %s\n", tab_entry->table_name);
						printf("Number of Columns        (num_columns) = %d\n", tab_entry->num_columns);
						printf("Column Descriptor Offset (cd_offset)   = %d\n", tab_entry->cd_offset);
            printf("Table PD Flags           (tpd_flags)   = %d\n\n", tab_entry->tpd_flags);

						if (report)
						{
							fprintf(fhandle, "Table PD size            (tpd_size)    = %d\n", tab_entry->tpd_size);
							fprintf(fhandle, "Table Name               (table_name)  = %s\n", tab_entry->table_name);
							fprintf(fhandle, "Number of Columns        (num_columns) = %d\n", tab_entry->num_columns);
							fprintf(fhandle, "Column Descriptor Offset (cd_offset)   = %d\n", tab_entry->cd_offset);
              fprintf(fhandle, "Table PD Flags           (tpd_flags)   = %d\n\n", tab_entry->tpd_flags);
						}

						/* Next, write the cd_entry information */
						for(i = 0, col_entry = (cd_entry*)((char*)tab_entry + tab_entry->cd_offset);
								i < tab_entry->num_columns; i++, col_entry++)
						{
							printf("Column Name   (col_name) = %s\n", col_entry->col_name);
							printf("Column Id     (col_id)   = %d\n", col_entry->col_id);
							printf("Column Type   (col_type) = %d\n", col_entry->col_type);
							printf("Column Length (col_len)  = %d\n", col_entry->col_len);
							printf("Not Null flag (not_null) = %d\n\n", col_entry->not_null);

							if (report)
							{
								fprintf(fhandle, "Column Name   (col_name) = %s\n", col_entry->col_name);
								fprintf(fhandle, "Column Id     (col_id)   = %d\n", col_entry->col_id);
								fprintf(fhandle, "Column Type   (col_type) = %d\n", col_entry->col_type);
								fprintf(fhandle, "Column Length (col_len)  = %d\n", col_entry->col_len);
								fprintf(fhandle, "Not Null Flag (not_null) = %d\n\n", col_entry->not_null);
							}
						}

						if (report)
						{
							fflush(fhandle);
							fclose(fhandle);
						}
					} // File open error
				} // Table not exist
			} // no semantic errors
		} // Invalid table name
	} // Invalid statement

  return rc;
}

int initialize_tpd_list()
{
	int rc = 0;
	FILE *fhandle = NULL;
//	struct _stat file_stat;
	struct stat file_stat;

  /* Open for read */
  /* Check if the file is being compiled properly */
  if((fhandle = fopen("dbfile.bin", "rbc")) == NULL)
	{
		if((fhandle = fopen("dbfile.bin", "wbc")) == NULL)
		{
			rc = FILE_OPEN_ERROR;
		}
    else
		{
			g_tpd_list = NULL;
			/* Allocate and zero-initialize array */
			g_tpd_list = (tpd_list*)calloc(1, sizeof(tpd_list));

			if (!g_tpd_list)
			{
				rc = MEMORY_ERROR;
			}
			else
			{
				/* get the list_size member of g_tpd_list and set the value to tpd_list */
				g_tpd_list->list_size = sizeof(tpd_list);
				fwrite(g_tpd_list, sizeof(tpd_list), 1, fhandle);
				fflush(fhandle);
				fclose(fhandle);
			}
		}
	}
	else
	{
		/* There is a valid dbfile.bin file - get file size */
    //		_fstat(_fileno(fhandle), &file_stat);
		fstat(fileno(fhandle), &file_stat);
		printf("dbfile.bin size = %d\n", file_stat.st_size);
		g_tpd_list = (tpd_list*)calloc(1, file_stat.st_size);

		if (!g_tpd_list)
		{
			rc = MEMORY_ERROR;
		}
		else
		{
			/* Reads data from the given stream into the array pointed to, by ptr */
			fread(g_tpd_list, file_stat.st_size, 1, fhandle);
			fflush(fhandle);
			fclose(fhandle);

			if (g_tpd_list->list_size != file_stat.st_size)
			{
				rc = DBFILE_CORRUPTION;
			}

		}
	}

	return rc;
}
/* Add tpd entry to the file */
int add_tpd_to_list(tpd_entry *tpd)
{
	int rc = 0;
	int old_size = 0;
	FILE *fhandle = NULL;

	if((fhandle = fopen("dbfile.bin", "wbc")) == NULL)
	{
		rc = FILE_OPEN_ERROR;
	}
  else
	{
		old_size = g_tpd_list->list_size;

		if (g_tpd_list->num_tables == 0)
		{
			/* If this is an empty list, overlap the dummy header */
			g_tpd_list->num_tables++;
		 	g_tpd_list->list_size += (tpd->tpd_size - sizeof(tpd_entry));
			fwrite(g_tpd_list, old_size - sizeof(tpd_entry), 1, fhandle);
		}
		else
		{
			/* There is at least 1, just append at the end */
			g_tpd_list->num_tables++;
		 	g_tpd_list->list_size += tpd->tpd_size;
			fwrite(g_tpd_list, old_size, 1, fhandle);
		}

		fwrite(tpd, tpd->tpd_size, 1, fhandle);
		fflush(fhandle);
		fclose(fhandle);
	}

	return rc;
}

int drop_tpd_from_list(char *tabname)
{
	int rc = 0;
	tpd_entry *cur = &(g_tpd_list->tpd_start);
	int num_tables = g_tpd_list->num_tables;
	bool found = false;
	int count = 0;

	if (num_tables > 0)
	{
		while ((!found) && (num_tables-- > 0))
		{
			if (strcasecmp(cur->table_name, tabname) == 0)
			{
				/* found it */
				found = true;
				int old_size = 0;
				FILE *fhandle = NULL;

				if((fhandle = fopen("dbfile.bin", "wbc")) == NULL)
				{
					rc = FILE_OPEN_ERROR;
				}
			  else
				{
					old_size = g_tpd_list->list_size;

					if (count == 0)
					{
						/* If this is the first entry */
						g_tpd_list->num_tables--;

						if (g_tpd_list->num_tables == 0)
						{
							/* This is the last table, null out dummy header */
							memset((void*)g_tpd_list, '\0', sizeof(tpd_list));
							g_tpd_list->list_size = sizeof(tpd_list);
							fwrite(g_tpd_list, sizeof(tpd_list), 1, fhandle);
						}
						else
						{
							/* First in list, but not the last one */
							g_tpd_list->list_size -= cur->tpd_size;

							/* First, write the 8 byte header */
							fwrite(g_tpd_list, sizeof(tpd_list) - sizeof(tpd_entry),
								     1, fhandle);

							/* Now write everything starting after the cur entry */
							fwrite((char*)cur + cur->tpd_size,
								     old_size - cur->tpd_size -
										 (sizeof(tpd_list) - sizeof(tpd_entry)),
								     1, fhandle);
						}
					}
					else
					{
						/* This is NOT the first entry - count > 0 */
						g_tpd_list->num_tables--;
					 	g_tpd_list->list_size -= cur->tpd_size;

						/* First, write everything from beginning to cur */
						fwrite(g_tpd_list, ((char*)cur - (char*)g_tpd_list),
									 1, fhandle);

						/* Check if cur is the last entry. Note that g_tdp_list->list_size
						   has already subtracted the cur->tpd_size, therefore it will
						   point to the start of cur if cur was the last entry */
						if ((char*)g_tpd_list + g_tpd_list->list_size == (char*)cur)
						{
							/* If true, nothing else to write */
						}
						else
						{
							/* NOT the last entry, copy everything from the beginning of the
							   next entry which is (cur + cur->tpd_size) and the remaining size */
							fwrite((char*)cur + cur->tpd_size,
										 old_size - cur->tpd_size -
										 ((char*)cur - (char*)g_tpd_list),
								     1, fhandle);
						}
					}

					fflush(fhandle);
					fclose(fhandle);
				}


			}
			else
			{
				if (num_tables > 0)
				{
					cur = (tpd_entry*)((char*)cur + cur->tpd_size);
					count++;
				}
			}
		}
	}

	if (!found)
	{
		rc = INVALID_TABLE_NAME;
	}

	return rc;
}
/* Return tpd from a list of token with specified tabname */
tpd_entry* get_tpd_from_list(char *tabname)
{
	tpd_entry *tpd = NULL;
	tpd_entry *cur = &(g_tpd_list->tpd_start);
	int num_tables = g_tpd_list->num_tables;
	bool found = false;

  /* Find the corresponding tables */
	if (num_tables > 0)
	{
		while ((!found) && (num_tables-- > 0))
		{
      /* Compare two string without worrying about upper or lower case */
			if (strcasecmp(cur->table_name, tabname) == 0)
			{
				/* found it */
				found = true;
				tpd = cur;
			}
			else
			{
        /* while there is more than 1 tables */
				if (num_tables > 0)
				{
          /* increment the pointer to the size of tpd */
					cur = (tpd_entry*)((char*)cur + cur->tpd_size);
				}
			}
		}
	}

	return tpd;
}


int sem_create_table(token_list *t_list)
{
	int rc = 0;
	token_list *cur;
  /* Table packed descriptor sturcture */
	tpd_entry tab_entry;
	tpd_entry *new_entry = NULL;
	bool column_done = false;
	int cur_id = 0;
  /* Column descriptor sturcture */
	cd_entry col_entry[MAX_NUM_COL];
  /* Pointer to the file in struct format */
  tabfile_ptr = NULL;

  /* Fill the memory blck from tab_entry + tpd_entry with \0.*/
	memset(&tab_entry, '\0', sizeof(tpd_entry));
	cur = t_list;

	if ((cur->tok_class != keyword) &&
		  (cur->tok_class != identifier) &&
			(cur->tok_class != type_name))
	{
		// Error
		rc = INVALID_TABLE_NAME;
		cur->tok_value = INVALID;
	}
	else /* There is a valid class */
	{
    /* check if that table already existed  */
		if ((new_entry = get_tpd_from_list(cur->tok_string)) != NULL)
		{
			rc = DUPLICATE_TABLE_NAME;
			cur->tok_value = INVALID;
		}
		else
		{
      /* Create the tab file with the table name */
      char filename[MAX_IDENT_LEN+5];
      memset(&filename, '\0', MAX_IDENT_LEN+5);
      strcpy(filename, cur->tok_string);
      strcat(filename, ".tab");

      /* Keep track of the record size as going through tokens */
      int tmp_record_size = 0;

      /* copy the table name string by its name into tab_entry */
      strcpy(tab_entry.table_name, cur->tok_string);

			cur = cur->next;
			if (cur->tok_value != S_LEFT_PAREN)
			{
				//Error
				rc = INVALID_TABLE_DEFINITION;
				cur->tok_value = INVALID;
			}
			else
			{
        /* Allocate some space in the memory with all 0  */
				memset(&col_entry, '\0', (MAX_NUM_COL * sizeof(cd_entry)));

				/* Now build a set of column entries by moving to the next pointer */
				cur = cur->next;
				do
				{
					if ((cur->tok_class != keyword) &&
							(cur->tok_class != identifier) &&
							(cur->tok_class != type_name))
					{
						// Error
						rc = INVALID_COLUMN_NAME;
						cur->tok_value = INVALID;
					}
					else
					{
						int i;
						for(i = 0; i < cur_id; i++)
						{
              /* make column name case sensitive */
              /* if these two string pointers are not the same */
							if (strcmp(col_entry[i].col_name, cur->tok_string)==0)
							{
								rc = DUPLICATE_COLUMN_NAME;
								cur->tok_value = INVALID;
								break;
							}
						}

						if (!rc)
						{
              /* copy the current.token_string to entry.col_name */
							strcpy(col_entry[cur_id].col_name, cur->tok_string);
							col_entry[cur_id].col_id = cur_id;
							col_entry[cur_id].not_null = false;    /* set default */

							cur = cur->next;
							if (cur->tok_class != type_name)
							{
								// Error
								rc = INVALID_TYPE_NAME;
								cur->tok_value = INVALID;
							}
							else /* Check for the column type  */
							{
                /* Set the column type here, int or char */
								col_entry[cur_id].col_type = cur->tok_value;
								cur = cur->next;

								if (col_entry[cur_id].col_type == T_INT)
								{
									if ((cur->tok_value != S_COMMA) &&
										  (cur->tok_value != K_NOT) &&
										  (cur->tok_value != S_RIGHT_PAREN))
									{
										rc = INVALID_COLUMN_DEFINITION;
										cur->tok_value = INVALID;
									}
								  else
									{
                    tmp_record_size = tmp_record_size + 1 + sizeof(int);
										col_entry[cur_id].col_len = sizeof(int);

										if ((cur->tok_value == K_NOT) &&
											  (cur->next->tok_value != K_NULL))
										{
											rc = INVALID_COLUMN_DEFINITION;
											cur->tok_value = INVALID;
										}
                    /* Check for not null */
										else if ((cur->tok_value == K_NOT) &&
											    (cur->next->tok_value == K_NULL))
										{
											col_entry[cur_id].not_null = true;
											cur = cur->next->next;
										}

										if (!rc)
										{
											/* I must have either a comma or right paren */
											if ((cur->tok_value != S_RIGHT_PAREN) &&
												  (cur->tok_value != S_COMMA))
											{
												rc = INVALID_COLUMN_DEFINITION;
												cur->tok_value = INVALID;
											}
											else
		                  {
												if (cur->tok_value == S_RIGHT_PAREN)
												{
 													column_done = true;
												}
												cur = cur->next;
											}
										}
									}
								}   // end of T_INT processing
								else
								{
									// It must be char() or varchar()
									if (cur->tok_value != S_LEFT_PAREN)
									{
										rc = INVALID_COLUMN_DEFINITION;
										cur->tok_value = INVALID;
									}
									else
									{
										/* Enter char(n) processing */
										cur = cur->next;

										if (cur->tok_value != INT_LITERAL)
										{
											rc = INVALID_COLUMN_LENGTH;
											cur->tok_value = INVALID;
										}
										else
										{
											/* Got a valid integer - convert */
											col_entry[cur_id].col_len = atoi(cur->tok_string);
                      tmp_record_size = tmp_record_size + 1 + col_entry[cur_id].col_len;

											cur = cur->next;

											if (cur->tok_value != S_RIGHT_PAREN)
											{
												rc = INVALID_COLUMN_DEFINITION;
												cur->tok_value = INVALID;
											}
											else
											{
												cur = cur->next;

												if ((cur->tok_value != S_COMMA) &&
														(cur->tok_value != K_NOT) &&
														(cur->tok_value != S_RIGHT_PAREN))
												{
													rc = INVALID_COLUMN_DEFINITION;
													cur->tok_value = INVALID;
												}
												else
												{
													if ((cur->tok_value == K_NOT) &&
														  (cur->next->tok_value != K_NULL))
													{
														rc = INVALID_COLUMN_DEFINITION;
														cur->tok_value = INVALID;
													}
													else if ((cur->tok_value == K_NOT) &&
																	 (cur->next->tok_value == K_NULL))
													{
														col_entry[cur_id].not_null = true;
														cur = cur->next->next;
													}

													if (!rc)
													{
														/* I must have either a comma or right paren */
														if ((cur->tok_value != S_RIGHT_PAREN) && (cur->tok_value != S_COMMA))
														{
															rc = INVALID_COLUMN_DEFINITION;
															cur->tok_value = INVALID;
														}
														else
													  {
															if (cur->tok_value == S_RIGHT_PAREN)
															{
																column_done = true;
															}
															cur = cur->next;
														}
													}
												}
											}
										}	/* end char(n) processing */
									}
								} /* end char processing */
							}
						}  // duplicate column name
					} // invalid column name

					/* If rc=0, then get ready for the next column */
					if (!rc)
					{
						cur_id++;
					}
        /* Continue doing it until there is no more column */
				} while ((rc == 0) && (!column_done));

        /* If there is no more column but it's not end of command */
				if ((column_done) && (cur->tok_value != EOC))
				{
					rc = INVALID_TABLE_DEFINITION;
					cur->tok_value = INVALID;
				}

				if (!rc)
				{
					/* Now finished building tpd entry and add it to the Table packed descriptor list (tpd) */
					tab_entry.num_columns = cur_id;
					tab_entry.tpd_size = sizeof(tpd_entry) +
															 sizeof(cd_entry) *	tab_entry.num_columns;
				  tab_entry.cd_offset = sizeof(tpd_entry);
          /* Allocate space for the comple tpd_entry */
          /* return the pointer to the allocated space */
					new_entry = (tpd_entry*)calloc(1, tab_entry.tpd_size);

          /* Round the record size to divisible by 4 */
          while(tmp_record_size % 4 != 0){
              tmp_record_size++;
          }

          /* Set all the value to the header and write*/
          FILE *fhandle = NULL;
          fhandle = fopen(filename, "wbc");
          tabfile_ptr = (table_file_header*)calloc(1, sizeof(table_file_header));
          tabfile_ptr->file_size = sizeof(table_file_header);
          tabfile_ptr->num_records = 0;
          tabfile_ptr->file_header_flag = 0;
          tabfile_ptr->record_size = tmp_record_size;
          tabfile_ptr->record_offset = tabfile_ptr->file_size;
          /* Intialize everything with starter column defintiion to the tab file */
          fwrite(tabfile_ptr, sizeof(table_file_header), 1, fhandle);
          fflush(fhandle);
          fclose(fhandle);

					if (new_entry == NULL)
					{
						rc = MEMORY_ERROR;
					}
					else
					{
            /* copies sizeof(tpd_entry) characters from memory area str2 to memory area str1.*/
						memcpy((void*)new_entry,
							     (void*)&tab_entry,
									 sizeof(tpd_entry));

						memcpy((void*)((char*)new_entry + sizeof(tpd_entry)),
									 (void*)col_entry,
									 sizeof(cd_entry) * tab_entry.num_columns);

						rc = add_tpd_to_list(new_entry);

						free(new_entry);
					}
          free(tabfile_ptr);
				}
			}
		}
	}
  return rc;
}


int sem_insert_record(token_list *t_list)
{
    int rc = 0;
    /* The current pointer to the token list */
	  token_list *cur;
    /* Temp table packed descriptor sturcture */
  	tpd_entry tab_entry;
    tpd_entry *new_entry = NULL;
    bool column_done = false;
    int cur_id = 0;
    /* Column descriptor sturcture */
    cd_entry *col_entry;
    // struct table_file_header_def tabfile;
    tabfile_ptr = NULL;
    FILE *fhandle = NULL;
    char filename[MAX_IDENT_LEN+5];

    /* Set the current pointer to the token list */
    cur = t_list;

    if ((cur->tok_class != keyword) &&
  		  (cur->tok_class != identifier) &&
  			(cur->tok_class != type_name))
  	{
  		// Error
  		rc = INVALID_TABLE_NAME;
  		cur->tok_value = INVALID;
  	}
    else /* There is a valid class */
    {
      /* if the table alerady existed then we can insert */
      if ((new_entry = get_tpd_from_list(cur->tok_string)) == NULL){
          rc = TABLE_NOT_EXIST;
          cur->tok_value = INVALID;
      }
      else
      {
          strcpy(tab_entry.table_name, cur->tok_string);
          strcpy(filename, cur->tok_string);
          strcat(filename, ".tab");

          cur = cur->next;
          /* check the token is VALUES */
          if (cur->tok_value != K_VALUES)
          {
            //Error
            rc = INSERT_SYNTAX_VALUE;
            printf("%s\n", "Keyword VALUES is missing" );
            cur->tok_value = INVALID;
          }
          else
    			{
             cur = cur->next;
             //If the insert statement doesn't have ( left paran
             if(cur->tok_value != S_LEFT_PAREN)
             {
               rc = INSERT_MISSING_COMA;
               printf("%s\n", "Missing ( ");
               cur->tok_value = INVALID;
             }
             else {
               //Go inside the paranthesis
                cur = cur->next;
                //Keep track of offset where buffer start
                int buffer_offset = 0;
                //Allocate record size one at the time
                char* record_ptr = (char *)load_data_from_tab((char *)tab_entry.table_name);

                //Cast to get the table_file_header struc
                tabfile_ptr = (table_file_header*)record_ptr;

                //Create a buffer to insert memory
                char buffer[tabfile_ptr->record_size];
                memset(buffer, '\0', tabfile_ptr->record_size);

                //Loop through the all the columns of selected table
                int i = 0;
                for(i = 0, col_entry = (cd_entry*)((char*)new_entry + new_entry->cd_offset);
								i < new_entry->num_columns; i++, col_entry++)
						    {
                    if(!rc){
                      //If it's NULL but it's not supposed to be NULL
                      if(cur->tok_value == K_NULL && col_entry->not_null == 1){
                          rc = INSERT_NOT_NULL_EXCEPTION;
                          cur->tok_value = INVALID;
                          printf("%s%s\n", "Not Null constraint exists for column name ", col_entry->col_name );
                      }
                      else { //It can accept NULL

                        //If it's NULL -> move to the next token
                        if(cur->tok_value == K_NULL){
                          buffer_offset += col_entry->col_len + 1;
                          cur = cur->next;
                          if(i < new_entry->num_columns - 1){
                              if(cur->tok_value == S_RIGHT_PAREN){
                                rc = INSERT_MISSING_COMA;
                                cur->tok_value = INVALID;
                                printf("%s\n", "Missing ) or the number of columns and insert values don't match");
                              }
                              else if(cur->tok_value != S_COMMA){
                                rc = INSERT_MISSING_COMA;
                                cur->tok_value = INVALID;
                                printf("%s\n", "Missing coma or the number of columns and insert values don't match");
                              }
                              else {
                                cur = cur->next;
                              }
                          }
                          else { // if its the last column
                              if(cur->tok_value != S_RIGHT_PAREN || cur->tok_value == S_COMMA){
                                rc = INSERT_MISSING_COMA;
                                cur->tok_value = INVALID;
                                printf("%s\n", "Missing ) or the number of columns and insert values don't match");
                              }
                              else {
                                column_done = true;
                                cur = cur->next;
                              }
                          }
                        }
                        //Reading in CHAR
                        else if(col_entry->col_type == T_CHAR)
                        {
                              if(cur->tok_value != STRING_LITERAL || cur->tok_class != constant)
                              {
                                  rc = INSERT_TYPE_MISMATCH;
                                  printf("%s\n", "Type mismatch");
                                  cur->tok_value = INVALID;
                              }
                              else { //if it's a valid string value
                                  //Write the length of the data
                                  int temp_len = strlen(cur->tok_string);
                                  unsigned char temp_len_chr = temp_len;
                                  unsigned char *p_len = &temp_len_chr;
                                  memcpy(buffer+buffer_offset, p_len, 1);
                                  buffer_offset++;

                                  //Write the content of the string
                                  memcpy(buffer+buffer_offset, cur->tok_string, col_entry->col_len);
                                  buffer_offset = buffer_offset + col_entry->col_len;

                                  //Check for comma or right paran
                                  cur = cur->next;
                                  // Not the last column
                                  if(i < new_entry->num_columns - 1){
                                      if(cur->tok_value == S_RIGHT_PAREN){
                                        rc = INSERT_MISSING_COMA;
                                        cur->tok_value = INVALID;
                                        printf("%s\n", "Missing ) or the number of columns and insert values don't match");
                                      }
                                      else if(cur->tok_value != S_COMMA){
                                        rc = INSERT_MISSING_COMA;
                                        cur->tok_value = INVALID;
                                        printf("%s\n", "Missing coma or the number of columns and insert values don't match");
                                      }
                                      else {
                                        cur = cur->next;
                                      }
                                  }
                                  else { // if its the last column
                                      if(cur->tok_value != S_RIGHT_PAREN || cur->tok_value == S_COMMA){
                                        rc = INSERT_MISSING_COMA;
                                        cur->tok_value = INVALID;
                                        printf("%s\n", "Missing ) or the number of columns and insert values don't match");
                                      }
                                      else {
                                        column_done = true;
                                        cur = cur->next;
                                      }
                                  }
                              }
                          }
                          //Reading in INT
                          else if(col_entry->col_type == T_INT)
                          {
                              //Check for invalid input
                              if(cur->tok_value != INT_LITERAL || cur->tok_class != constant)
                              {
                                  rc = INSERT_TYPE_MISMATCH;
                                  cur->tok_value = INVALID;
                                  printf("%s\n", "Type mismatch");
                              }
                              else { // if its a valid int value , parse the int and then check for comma
                                int temp_len = sizeof(int);
                                unsigned char temp_len_chr = temp_len;
                                unsigned char *p_len = &temp_len_chr;
                                memcpy(buffer+buffer_offset, p_len, 1);
                                buffer_offset = buffer_offset + 1;

                                //Write the actual int
                                int temp_int = atoi(cur->tok_string);
                                int *p_int = &temp_int;
                                memcpy(buffer+buffer_offset, p_int, col_entry->col_len );
                                buffer_offset = buffer_offset + col_entry->col_len ;

                                cur = cur->next;
                                if(i < new_entry->num_columns - 1){
                                    if(cur->tok_value == S_RIGHT_PAREN){
                                      rc = INSERT_MISSING_COMA;
                                      cur->tok_value = INVALID;
                                      printf("%s\n", "Missing ) or the number of columns and insert values don't match");
                                    }
                                    else if(cur->tok_value != S_COMMA){
                                      rc = INSERT_MISSING_COMA;
                                      cur->tok_value = INVALID;
                                      printf("%s\n", "Missing coma or the number of columns and insert values don't match");
                                    }
                                    else {
                                      cur = cur->next;
                                    }
                                }
                                else { // if its the last column
                                    if(cur->tok_value != S_RIGHT_PAREN || cur->tok_value == S_COMMA){
                                      rc = INSERT_MISSING_COMA;
                                      cur->tok_value = INVALID;
                                      printf("%s\n", "Missing ) or the number of columns and insert values don't match");
                                    }
                                    else {
                                      column_done = true;
                                      cur = cur->next;
                                    }
                                }
                              }
                          }//End checking for INT
                      }//Check for NOT NULL
                    }//Check for invalid rc
                }//End of for loop
                if ((column_done) && (cur->tok_value != EOC))
                {
                  rc = INVALID_TABLE_DEFINITION;
                  cur->tok_value = INVALID;
                }
                if (!rc)
                {
                  /* Now finished writing to buffer and write to file */
                  fhandle = fopen(filename,"a+bc");
                  fwrite(buffer, tabfile_ptr->record_size, 1, fhandle);
                  fflush(fhandle);
                  fclose(fhandle);

                  /* Update header with correct information  */
                  tabfile_ptr->file_size = tabfile_ptr->file_size + tabfile_ptr->record_size;
                  tabfile_ptr->num_records++;
                  fhandle = fopen(filename,"r+bc");
                  fwrite(tabfile_ptr, tabfile_ptr->record_offset, 1, fhandle);
                  fflush(fhandle);
                  fclose(fhandle);
                  printf("%s size: %d. Number of records: %d\n", filename, tabfile_ptr->file_size, tabfile_ptr->num_records);
                }
             }
          }
      }
    }
    return rc;
}

int sem_select_all(token_list *t_list) {
    token_list *cur;
    tpd_entry *tab_entry = NULL;
    cd_entry *col_entry = NULL;
    FILE *fhandle = NULL;

    tabfile_ptr = NULL;

    int cur_row = 0;
    int rc = 0;

    struct stat file_stat;
    char* record_ptr = NULL;

    /* Set the current pointer to the token list */
    cur = t_list;
    /* Check if it has the valid syntax - FROM */
    if(cur->tok_value != K_FROM){
      rc = INVALID_SELECT_ALL_SYNTAX;
      cur->tok_value = INVALID;
    }
    else {
      cur = cur->next;
       /* Check for table name */
      if ((cur->tok_class != keyword) &&
          (cur->tok_class != identifier) &&
          (cur->tok_class != type_name))
      {
        rc = INVALID_TABLE_NAME;
        cur->tok_value = INVALID;
      }
      else { /* Check for table name */
        if ((tab_entry = get_tpd_from_list(cur->tok_string)) != NULL)
        {
          struct cd_entry_def *list_cd_entry[tab_entry->num_columns];
          record_ptr = load_data_from_tab(cur->tok_string);
            if (!record_ptr)
            {
              rc = MEMORY_ERROR;
            }
            else
            {
              printf("%s\n", "AFTER");
              tabfile_ptr = (table_file_header*)record_ptr;
              record_ptr+= tabfile_ptr->record_offset;

              //Start printing out results
              int count = 0;
              for( count = 0; count < tab_entry->num_columns; count++){
                printf("%s","+----------------");
              }
              printf("+\n");

              int i = 0;
              for(i = 0, col_entry = (cd_entry*)((char*)tab_entry + tab_entry->cd_offset);
                  i < tab_entry->num_columns; i++, col_entry++)
              {
                  list_cd_entry[i] = col_entry;
                  printf("|%-16s", col_entry->col_name);
                  if(i == tab_entry->num_columns - 1) {
                    printf("|\n");
                  }
              }

              for( count = 0; count < tab_entry->num_columns; count++){
                printf("%s","+----------------");
              }
              printf("+\n");

              int record_offset = 0;
              for(cur_row = 0; cur_row < tabfile_ptr->num_records; cur_row++){
                record_offset = 0;
                  for(i = 0; i < tab_entry->num_columns; i++){
                    printf("%s","|");
                    int temp_len = 0;
                    memcpy(&temp_len, record_ptr, 1);
                    record_offset++;
                    /* If the column type is CHAR*/
                     if(list_cd_entry[i]->col_type == T_CHAR || list_cd_entry[i]->col_type == T_VARCHAR){
                       if(temp_len == 0){ //NULL
                          printf("%-16s","NULL");
                          record_offset = record_offset + list_cd_entry[i]->col_len;
                       }
                       else {
                         char temp_string[temp_len];
                         memset(temp_string, '\0', temp_len);
                         memcpy(&temp_string, record_ptr+record_offset, list_cd_entry[i]->col_len);
                         printf("%-16s", temp_string);
                         record_offset = record_offset + list_cd_entry[i]->col_len;
                       }
                     }
                     else { //column type is INT
                       if(temp_len == 0){ //NULL
                         printf("%16s","NULL");
                         record_offset = record_offset + sizeof(int);
                       }
                       else {
                         int value = 0;
                         memcpy(&value, record_ptr+record_offset, sizeof(int));
                         record_offset = record_offset + sizeof(int);
                         printf("%16d",value);
                       }
                     }
                  }
                  record_ptr = record_ptr + tabfile_ptr->record_size;
                  printf("|\n");
              }
              for( count = 0; count < tab_entry->num_columns; count++){
                printf("%s","+----------------");
              }
              printf("+\n");
              fflush(fhandle);
              fclose(fhandle);
            }
        }
        if(!rc)
          {
            cur = cur->next; //Should be the terminator
            if (cur->tok_value != EOC)
              {
                rc = INVALID_TABLE_DEFINITION;
                cur->tok_value = INVALID;
              }//End checking for terminator
          }
        }//End checking if that table exist
    }// End checking for invalid table
    return rc;
}

char* load_data_from_tab(char *tablename){
  struct stat file_stat;
  FILE *fhandle = NULL;
  char *result = NULL;
  char filename[MAX_IDENT_LEN+5];
  int rc = 0;

  /* Concat .tab to table name */
  strcpy(filename, tablename);
  strcat(filename, ".tab");
  /* Trying to read the fhandle */
  if((fhandle = fopen(filename, "rbc")) != NULL){
    //Bring the data to memory
      fstat(fileno(fhandle), &file_stat);
      result = (char*)calloc(1, file_stat.st_size);
      if (!result)
      {
        rc = MEMORY_ERROR;
      }
      else
      {
        fread(result, file_stat.st_size, 1, fhandle);
        fflush(fhandle);
        fclose(fhandle);
      }
  }
  else {
    rc = FILE_OPEN_ERROR;
  }
  return result;
}
