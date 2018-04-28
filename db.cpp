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
        (cur->next != NULL))
  {
    printf("SELECT statement\n");
    cur_cmd = SELECT;
    cur = cur->next;
  }
  else if ((cur->tok_value == K_DELETE) &&
        ((cur->next != NULL) && (cur->next->tok_value == K_FROM)))
  {
    printf("DELETE statement\n");
    cur_cmd = DELETE;
    cur = cur->next;
  }
  else if (cur->tok_value == K_UPDATE)
  {
    printf("UPDATE statement\n");
    cur_cmd = UPDATE;
    cur = cur->next;
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
            rc = sem_select(cur);
            break;
      case DELETE:
            rc = sem_delete(cur);
            break;
      case UPDATE:
            rc = sem_update(cur);
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
                                  int tok_length = strlen(cur->tok_string);
                                  unsigned char temp_len_chr = tok_length;
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
                                int tok_length = sizeof(int);
                                unsigned char temp_len_chr = tok_length;
                                unsigned char *p_len = &temp_len_chr;
                                memcpy(buffer+buffer_offset, p_len, 1);
                                buffer_offset++;
                        				long long temp_ll = atoll(cur->tok_string);
                        				if(temp_ll > 2147483647)
                        				{
                        				    rc = INSERT_INVALID_VALUE;
                         				    cur->tok_value = INVALID;
                        				    printf("%s\n", "Exceed max integer value");
                        				}
                        				else{
                        				//Write the actual int
                        				int temp_int = atoi(cur->tok_string);
                                int *p_int = &temp_int;
                                memcpy(buffer+buffer_offset, p_int, col_entry->col_len );
                                buffer_offset += col_entry->col_len ;

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

int sem_select(token_list *t_list) {
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

    token_list *cur_projection = NULL;
    token_list *aggregate_func = NULL;
    int count_proj = 0;
    char proj_col[MAX_NUM_COL][MAX_TOK_LEN];

    bool select_all = false;
    //Check for SELECT ALL
    if(cur->tok_value == S_STAR){
        select_all = true;
        cur = cur->next;
        if(cur->tok_value != K_FROM){
          rc = INVALID_SELECT_ALL_SYNTAX;
          printf("%s\n", "Invalid SELECT ALL query");
          cur->tok_value = INVALID;
          return rc;
        }
    }
    else { //must be other values
      if(cur->tok_value == K_FROM){
        rc = INVALID_SELECT_ALL_SYNTAX;
        cur->tok_value = INVALID;
      }
      else {
        //check for aggregate functions
        if(cur->tok_value == F_SUM || cur->tok_value == F_AVG || cur->tok_value == F_COUNT) { //Has an aggregate
          aggregate_func = cur;
          cur = cur->next; //This must be ()
          if(cur->tok_value != S_LEFT_PAREN){
            rc = INVALID_SELECT_ALL_SYNTAX;
            cur->tok_value = INVALID;
            printf("%s\n", "Invalid or missing (");
          }
          else { // continue parsing projection column
               cur = cur->next;
               if(cur->tok_value != IDENT){
                 rc = INVALID_SELECT_ALL_SYNTAX;
                 cur->tok_value = INVALID;
                 printf("%s\n", "Invalid or missing column name");
               }
               else{
                 cur_projection = cur;
                 /* Check for projection column */
                 while(cur->tok_value != S_RIGHT_PAREN){
                   strcpy(proj_col[count_proj], cur->tok_string);
                   // printf("Array %s Count %d\n",proj_col[count_proj],count_proj );
                   count_proj++;
                   cur = cur->next;
                   if(cur->tok_value == S_COMMA && cur->next != NULL){
                     cur = cur->next;
                   }
                 }
                 if(cur->tok_value == S_RIGHT_PAREN){
                    cur = cur->next; //to move to FROM
                    if(cur->tok_value != K_FROM){
                      rc = INVALID_SELECT_ALL_SYNTAX;
                      cur->tok_value = INVALID;
                      printf("%s\n", "Invalid or missing FROM");
                    }
                 }
                 else {
                   rc = INVALID_SELECT_ALL_SYNTAX;
                   cur->tok_value = INVALID;
                   printf("%s\n", "Invalid or missing )");
                 }
               }
          }
        }//End parsing aggregate
        else if(cur->tok_value == IDENT){
          cur_projection = cur;
          /* Check for projection column */
          while(cur->tok_value != K_FROM){
            strcpy(proj_col[count_proj], cur->tok_string);
            // printf("Array %s Count %d\n",proj_col[count_proj],count_proj );
            count_proj++;
            cur = cur->next;
            if(cur->tok_value == S_COMMA && cur->next != NULL){
              cur = cur->next;
            }
          }
        }//End parsing column name
        else {
          rc = INVALID_SELECT_ALL_SYNTAX;
          cur->tok_value = INVALID;
          printf("%s\n", "Invalid or missing column name");
        }
      }
    }
    if(!rc){
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
              /* Load file to memory  */
              record_ptr = load_data_from_tab(cur->tok_string);
              if (!record_ptr)
              {
                rc = MEMORY_ERROR;
              }
              else
              {
                /* Load file to memory  */
                tabfile_ptr = (table_file_header*)record_ptr;
                /* Jump to the first record  */
                record_ptr+= tabfile_ptr->record_offset;
                struct cd_entry_def *list_cd_entry[tab_entry->num_columns];

                int count_cond = 0;
                //Store the column name
                token_list *cond_column[2];
                //Store the signs or null
                token_list *cond_relation_operator[2];
                //Store the data value
                char *cond_value[2];
                token_list *orderby_column;
                // int *cond_value_int[2];

                bool where_flag = false;
                bool orderby_flag = false;
                bool multi_cond = false;
                token_list *and_or = NULL;
                bool order_desc = false;
                cd_entry *orderby_col;
                /*Check for WHERE Clause */
                token_list *cur2 = NULL;
                if(cur->next != NULL && cur->next->tok_value == K_WHERE){
                    where_flag = true;
                    cur = cur->next;
                    if(cur->next == NULL || cur->next->tok_value == EOC){
                      rc = INVALID_SELECT_ALL_SYNTAX;
                      cur->tok_value = INVALID;
                      printf("%s\n", "Invalid WHERE syntax");
                      return rc;
                    }
                    cur2 = cur->next; // jump to column
                    do{ //Check each condition
                          if(cur2 != NULL && cur2->tok_value == IDENT){ //Start scanning for column
                            cond_column[count_cond] = cur2;
                            printf("Column %d: %s\n",count_cond,cond_column[count_cond]->tok_string); //condition
                            if(cur2->next != NULL && cur2->next->tok_value != EOC){
                              cur2 = cur2->next;
                              if(cur2->tok_value == S_EQUAL || cur2->tok_value == S_LESS || cur2->tok_value == S_GREATER){
                                  //belong to the signs
                                  printf("%s\n", "Sign");
                                  cond_relation_operator[count_cond] = cur2;
                                  printf("%d\n", cond_relation_operator[count_cond]->tok_value);
                                  if(cur2->next != NULL && cur2->next->tok_value != EOC){
                                      cur2 = cur2->next;
                                      if(cur2->tok_value == STRING_LITERAL){ //char
                                         cond_value[count_cond] = cur2->tok_string;
                                         printf("%s\n", cond_value[count_cond]);
                                      }
                                      else if(cur2->tok_value == INT_LITERAL){ //must be int
                                         printf("%s \n", cur2->tok_string);
                                         int temp_int = atoi(cur2->tok_string);
                                         int *p_int = &temp_int;
                                         cond_value[count_cond] = (char*)p_int;
                                         int *back = (int*)cond_value[count_cond];
                                         printf("%d\n", *back);
                                      }
                                      else {
                                          if(cur2->tok_value == K_AND || cur2->tok_value == K_OR){
                                            rc = INVALID_SELECT_ALL_SYNTAX;
                                            cur2->tok_value = INVALID;
                                            printf("%s\n", "Syntax doesn't match any relational operator");
                                            return rc;
                                          }
                                      }
                                  }
                                  else{
                                    rc = INVALID_SELECT_ALL_SYNTAX;
                                    cur2->tok_value = INVALID;
                                    printf("%s\n", "Syntax doesn't match any relational operator");
                                    return rc;
                                  }
                              }
                              else if(cur2->tok_value == K_IS){
                                // is null
                                if(cur2->next != NULL && cur2->next->tok_value == K_NOT){
                                  cur2 = cur2->next;
                                  if(cur2->next != NULL && cur2->next->tok_value == K_NULL)
                                  {
                                    printf("%s\n", "Is Not Null");
                                    cond_relation_operator[count_cond] = cur2;
                                    printf("%d\n", cond_relation_operator[count_cond]->tok_value);
                                    cur2 = cur2->next;
                                  }
                                  else {

                                    rc = INVALID_SELECT_ALL_SYNTAX;
                                    cur2->next->tok_value = INVALID;
                                    printf("%s\n", "Syntax doesn't match IS NOT NULL condition");
                                    return rc;
                                  }
                                }
                                else if(cur2->next != NULL && cur2->next->tok_value == K_NULL)
                                {
                                  printf("%s\n", "Is Null");
                                  cond_relation_operator[count_cond] = cur2->next;
                                  printf("%d\n", cond_relation_operator[count_cond]->tok_value);
                                  cur2 = cur2->next;

                                }
                                else {
                                  rc = INVALID_SELECT_ALL_SYNTAX;
                                  cur2->next->tok_value = INVALID;
                                  printf("%s\n", "Syntax doesn't match IS NULL condition");
                                  return rc;
                                }
                              }
                              if(cur2->tok_value == EOC){
                                rc = INVALID_SELECT_ALL_SYNTAX;
                                cur2->tok_value = INVALID;
                                printf("%s\n", "Syntax doesn't match any condition");
                                return rc;
                              }
                            }
                            else {
                               rc = INVALID_SELECT_ALL_SYNTAX;
                               cur2->tok_value = INVALID;
                               printf("%s\n", "Incomplete condition");
                               return rc;
                            }
                          }
                          if(!rc && !multi_cond){
                             if(cur2->next != NULL
                                && (cur2->next->tok_value == K_AND || cur2->next->tok_value == K_OR))
                              {
                                  and_or = cur2->next;
                                  multi_cond = true;
                                  cur2 = and_or;
                                  if(cur2->next->tok_value == EOC){
                                    rc = INVALID_SELECT_ALL_SYNTAX;
                                    cur2->tok_value = INVALID;
                                    printf("%s\n", "Incomplete condition");
                                    return rc;
                                  }
                                  else{
                                    cur2 = and_or->next;
                                  }
                            }
                          }
                          count_cond++;
                    }while(multi_cond && count_cond < 2);
                    //Check for order by in where
                    if(cur2->next != NULL && cur2->next->tok_value == K_ORDER){
                      if(cur2->next->next != NULL && cur2->next->next->tok_value == K_BY){
                          printf("%d\n", cur->next->next->next->tok_value);
                          if(cur2->next->next->next->tok_value == IDENT){
                              orderby_column = cur2->next->next->next;
                              if(orderby_column->next->tok_value == K_DESC){
                                 order_desc = true;
                              }
                              else if(orderby_column->next->tok_value != EOC){
                                rc = INVALID_SELECT_ALL_SYNTAX;
                                orderby_column->next->tok_value = INVALID;
                                printf("%s\n", "Invalid ORDER BY condition");
                                return rc;
                              }
                              orderby_flag = true;
                              printf("%s\n", "Order By Clause");
                          }
                          else {
                            rc = INVALID_SELECT_ALL_SYNTAX;
                            cur2->next->next->tok_value = INVALID;
                            printf("%s\n", "Missing or Invalid Column Name");
                            return rc;
                          }
                      }
                      else {
                        rc = INVALID_SELECT_ALL_SYNTAX;
                        cur2->next->next->tok_value = INVALID;
                        printf("%s\n", "Invalid Order By Syntax");
                        return rc;
                      }
                    }
                }
                //Check for order by
                else if(cur->next != NULL && cur->next->tok_value == K_ORDER){
                    if(cur->next->next != NULL && cur->next->next->tok_value == K_BY){
                        printf("%d\n", cur->next->next->next->tok_value);
                        if(cur->next->next->next->tok_value == IDENT){
                            orderby_column = cur->next->next->next;
                            if(orderby_column->next->tok_value == K_DESC){
                               order_desc = true;
                               if(orderby_column->next->next->tok_value != EOC){
                                 rc = INVALID_SELECT_ALL_SYNTAX;
                                 orderby_column->next->next->tok_value = INVALID;
                                 printf("%s\n", "Invalid SELECT statement");
                                 return rc;
                               }
                            }
                            else if(orderby_column->next->tok_value != EOC){
                              rc = INVALID_SELECT_ALL_SYNTAX;
                              orderby_column->next->tok_value = INVALID;
                              printf("%s\n", "Invalid ORDER BY condition");
                              return rc;
                            }
                            orderby_flag = true;
                            printf("%s\n", "Order By Clause");
                        }
                        else {
                          rc = INVALID_SELECT_ALL_SYNTAX;
                          cur->next->next->next->tok_value = INVALID;
                          printf("%s\n", "Invalid Column Name");
                          return rc;
                        }
                    }
                    else {
                      rc = INVALID_SELECT_ALL_SYNTAX;
                      cur->next->next->tok_value = INVALID;
                      printf("%s\n", "Invalid Order By Syntax");
                      return rc;
                    }
                }
                else {
                  if(cur->next->tok_value != EOC){
                    rc = INVALID_SELECT_ALL_SYNTAX;
                    cur->next->tok_value = INVALID;
                    printf("%s\n", "Invalid Syntax Error");
                    return rc;
                  }
                }

                int i = 0;
                //Get all the column information from file
                for(i = 0, col_entry = (cd_entry*)((char*)tab_entry + tab_entry->cd_offset);
                    i < tab_entry->num_columns; i++, col_entry++)
                {
                  list_cd_entry[i] = col_entry;
                }
                int print_column = tab_entry->num_columns;

                //Check agaisnt projection columns
                if(!select_all){
                  int col = 0;
                  bool match = false;
                  for (col = 0; col < count_proj; col++){ // check all the projection column
                    match = false;
                    for(i = 0; i < tab_entry->num_columns; i++) // against all the column in table
                    {
                      if(strcasecmp(proj_col[col], list_cd_entry[i]->col_name)==0){
                        match = true;
                        if(list_cd_entry[i]->col_type == T_CHAR || list_cd_entry[i]->col_type == T_VARCHAR){
                          if(aggregate_func != NULL && (aggregate_func->tok_value == F_SUM || aggregate_func->tok_value == F_AVG)){
                             rc = INVALID_SELECT_ALL_SYNTAX;
                             cur_projection->tok_value = INVALID;
                             printf("%s\n", "Can't apply SUM or AVG to String Column Type");
                             return rc;
                          }
                        }
                      }
                    }
                    if(!match){
                      rc = INVALID_SELECT_ALL_SYNTAX;
                      cur_projection->tok_value = INVALID;
                      return rc;
                    }
                    if(cur_projection){
                      cur_projection = cur_projection->next->next;
                    }
                  }
                  print_column = count_proj;
                }


                //Check where columns
                // int col = 0;
                // bool valid_column = true;
                // token_list *invalid_column;
                // if(where_flag){
                //   for(i = 0; i < tab_entry->num_columns; i++) // against all the column in table
                //   {
                //       if(cond_column[0]){
                //         if(strcasecmp(cond_column[0]->tok_string, list_cd_entry[i]->col_name) != 0
                //           && i == tab_entry->num_columns - 1 && valid_column == true){
                //             valid_column = false;
                //             invalid_column = cond_column[0];
                //         }
                //       }
                //       if(cond_column[1]){
                //           if(strcasecmp(cond_column[1]->tok_string, list_cd_entry[i]->col_name ) != 0
                //             && i == tab_entry->num_columns - 1 && valid_column == true){
                //             valid_column = false;
                //             invalid_column = cond_column[1];
                //           }
                //       }
                //       if(orderby_flag) {
                //         if(strcasecmp(orderby_column->tok_string, list_cd_entry[i]->col_name) != 0
                //           && i == tab_entry->num_columns - 1  && valid_column == true){
                //           valid_column = false;
                //           invalid_column = orderby_column;
                //
                //         }
                //       }
                //   }
                // }

                // if (!valid_column) {
                //   rc = INVALID_SELECT_ALL_SYNTAX;
                //   invalid_column->tok_value = INVALID;
                //   printf("The column %s is invalid\n",invalid_column->tok_string);
                //   return rc;
                // }

                if(aggregate_func != NULL){
                  print_column = 1;
                }

                //If everything is ok -> start printing
                for(int count = 0; count < print_column; count++){
                  printf("%s","+--------------------");
                }
                printf("+\n");

                /* Print column name  */
                for(i = 0; i < print_column; i++)
                {
                    //Start printing out results
                    if(!select_all){
                      if(aggregate_func != NULL){
                        char *end = ")";
                        char *start = "";
                        char combine[MAX_TOK_LEN];
                        if(aggregate_func->tok_value == F_SUM){
                             start = "SUM(";
                        }
                        else if(aggregate_func->tok_value == F_COUNT){
                             start = "COUNT(";
                        }
                        else if(aggregate_func->tok_value == F_AVG){
                             start = "AVG(";
                        }
                        strcpy(combine, start);
                        strcat(combine, proj_col[i]);
                        strcat(combine, end);
                        printf("|%*s", -FORMAT_LENGTH, combine);
                      }
                      else{
                        printf("|%*s", -FORMAT_LENGTH, proj_col[i]);
                      }
                    }
                    else {
                      printf("|%*s", -FORMAT_LENGTH, list_cd_entry[i]->col_name);
                    }
                }
                printf("|\n");

                for(int count = 0; count < print_column; count++){
                  printf("%s","+--------------------");
                }
                printf("+\n");


                //Start printing data
                int j = 0;
                int record_offset = 0;
                int print_count = 0;
                int total_row = 0;
                int row_select = 0;
                int row_select2 = 0;
                bool done = true;
                struct cd_entry_def *column_address;
                int orderby_offset = 0;
                char* record_to_print[tabfile_ptr->num_records]; //Array of all possible records to print;
                char* record_to_print2[tabfile_ptr->num_records]; //Array of all possible records to print;
                char* record_to_print_final[tabfile_ptr->num_records];
                /* For every row in this file */
                for(cur_row = 0; cur_row < tabfile_ptr->num_records; cur_row++){
                record_offset = 0;
                    /* For every column in the column */
                    for(j = 0; j < tab_entry->num_columns; j++){
                      column_address = list_cd_entry[j];
                      unsigned char tok_length = NULL;
                      memcpy(&tok_length, record_ptr+record_offset, 1);
                      record_offset++;

                      //If there is a where flag
                      if(where_flag){
                        if(strcasecmp(cond_column[0]->tok_string, column_address->col_name) == 0){
                          if(cond_relation_operator[0]->tok_value == K_NULL && tok_length == 0){ //when its null & check if is null
                            record_to_print[row_select] = record_ptr;
                            row_select++;
                          }//end checking for null
                          else if(cond_relation_operator[0]->tok_value == K_NOT && tok_length != 0){
                            record_to_print[row_select] = record_ptr;
                            row_select++;
                          }
                          else { // must be relational
                            if(column_address->col_type == T_CHAR || column_address->col_type == T_VARCHAR) // VARCHAR
                            {
                              char temp_string[tok_length + 1];
                              memset(temp_string, '\0', tok_length + 1);
                              memcpy(&temp_string, record_ptr+record_offset, tok_length);
                                  // printf("%s\n", temp_string);
                              if(cond_relation_operator[0]->tok_value == S_EQUAL){
                                if(strcasecmp(cond_value[0], temp_string) == 0){
                                    record_to_print[row_select] = record_ptr;
                                    row_select++;
                                }
                              }
                              else { //REDO - MOVE TO TOP
                                  rc = INVALID_SELECT_ALL_SYNTAX;
                                  cond_relation_operator[0]->tok_value = INVALID;
                                  printf("%s\n", "Can't have > or < for column type String");
                                  return rc;
                              }
                            }
                            else{ // this must be int
                              int temp_num = 0;
                              memcpy(&temp_num,record_ptr+record_offset, sizeof(int));
                              int *extract_value = (int*)cond_value[0];
                              if(cond_relation_operator[0]->tok_value == S_EQUAL
                                && temp_num == *extract_value){
                                  record_to_print[row_select] = record_ptr;
                                  row_select++;
                                }
                              else if(cond_relation_operator[0]->tok_value == S_GREATER
                                && temp_num > *extract_value)
                                {
                                  record_to_print[row_select] = record_ptr;
                                  row_select++;
                                }
                              else if(cond_relation_operator[0]->tok_value == S_LESS
                                && temp_num < *extract_value)
                                {
                                  record_to_print[row_select] = record_ptr;
                                  row_select++;
                                }
                            }
                          }
                        }
                        if(multi_cond){//if both condition
                            int boolean = and_or->tok_value;
                            //If match the column name -> find the matching cell
                            if(strcasecmp(cond_column[1]->tok_string, column_address->col_name) == 0){
                              if(cond_relation_operator[1]->tok_value == K_NULL && tok_length == 0){ //when its null & check if is null
                                record_to_print2[row_select2] = record_ptr;
                                row_select2++;
                              }//end checking for null
                              else if(cond_relation_operator[1]->tok_value == K_NOT && tok_length != 0){
                                record_to_print2[row_select2] = record_ptr;
                                row_select2++;
                              }
                              else { // must be relational
                                if(column_address->col_type == T_CHAR || column_address->col_type == T_VARCHAR) // VARCHAR
                                {
                                  char temp_string[tok_length + 1];
                                  memset(temp_string, '\0', tok_length + 1);
                                  memcpy(&temp_string, record_ptr+record_offset, tok_length);
                                      // printf("%s\n", temp_string);
                                  if(cond_relation_operator[1]->tok_value == S_EQUAL){
                                    if(strcasecmp(cond_value[1], temp_string) == 0){
                                        record_to_print2[row_select2] = record_ptr;
                                        row_select2++;
                                    }
                                  }
                                  else { //REDO - MOVE TO TOP
                                      rc = INVALID_SELECT_ALL_SYNTAX;
                                      cond_relation_operator[1]->tok_value = INVALID;
                                      printf("%s\n", "Can't have > or < for column type String");
                                      return rc;
                                  }
                                }
                                else{ // this must be int
                                  int temp_num = 0;
                                  memcpy(&temp_num,record_ptr+record_offset, sizeof(int));
                                  int *extract_value = (int*)cond_value[1];
                                  if(cond_relation_operator[1]->tok_value == S_EQUAL
                                    && temp_num == *extract_value){
                                      record_to_print2[row_select2] = record_ptr;
                                      row_select2++;
                                    }
                                  else if(cond_relation_operator[1]->tok_value == S_GREATER
                                    && temp_num > *extract_value)
                                    {
                                      record_to_print2[row_select2] = record_ptr;
                                      row_select2++;
                                    }
                                  else if(cond_relation_operator[1]->tok_value == S_LESS
                                    && temp_num < *extract_value)
                                    {
                                      record_to_print2[row_select2] = record_ptr;
                                      row_select2++;
                                    }
                                }
                              }
                            }
                        }
                        else {
                          record_to_print_final[row_select-1] = record_to_print[row_select-1];
                          total_row=row_select;
                        }
                      }
                      if(orderby_flag){
                        if(strcasecmp(orderby_column->tok_string, column_address->col_name) == 0){
                            orderby_col = column_address;
                            orderby_offset = record_offset;
                        }
                      }
                      record_offset = record_offset + list_cd_entry[j]->col_len;
                      // printf("Offset %d Length %d\n", record_offset, list_cd_entry[j]->col_len);
                    }
                    if(!where_flag){
                      record_to_print_final[cur_row] = record_ptr;
                      total_row++;
                    }
                    //At the end of each column
                    record_ptr = record_ptr + tabfile_ptr->record_size;
                    // printf("Print Count %d Count Proj %d", print_count, count_proj);
                }


                //Handle AND and OR statement
                if(multi_cond){
                  if(and_or->tok_value == K_AND){
                      for(int m = 0; m < row_select2; m++){
                        for(int n = 0; n < row_select; n++){
                          if(record_to_print2[m] == record_to_print[n]){
                            record_to_print_final[total_row++] = record_to_print2[m];
                            m++;
                          }
                        }
                      }
                  }else if(and_or->tok_value == K_OR){
                    for(int i = 0; i < row_select; i++){
                      record_to_print_final[i] = record_to_print[i];
                      total_row++;
                    }
                    for(int m = 0; m < row_select2; m++){
                      bool found = false;
                      for(int n = 0; n < row_select; n++){
                        if(record_to_print2[m] == record_to_print[n]){
                            found = true;
                            n++;
                        }
                      }
                      if(!found){
                        record_to_print_final[total_row++] = record_to_print2[m];
                      }
                    }
                  }
                }

                //ORDER BY LOGIC
                if(orderby_flag){
                  // printf("%d\n", orderby_col->col_type);
                  // printf("%d\n", orderby_offset);
                  // printf("%s\n", "here");
                  for(int i = 0; i < total_row - 1; i++){ // for the first row
                    //Sort String Now
                      if(orderby_col->col_type == T_CHAR || orderby_col->col_type == T_VARCHAR){
                        for(int j = 0; j < total_row - 1; j++){ // for the first row
                          int result = strcasecmp(record_to_print_final[j]+orderby_offset,record_to_print_final[j+1]+orderby_offset);
                            if(!order_desc && result > 0){ //the second is greater than the first
                                  //need to swap
                                  char *temp = record_to_print_final[j];
                                  record_to_print_final[j] = record_to_print_final[j+1];
                                  record_to_print_final[j+1] = temp;
                            }
                            else if(order_desc && result < 0){
                                  char *temp = record_to_print_final[j];
                                  record_to_print_final[j] = record_to_print_final[j+1];
                                  record_to_print_final[j+1] = temp;
                            }
                          }
                      }//END String
                      //Sort Int Now
                      else if(orderby_col->col_type == T_INT) {
                        for(int j = 0; j < total_row - 1; j++){ // for the first row
                           int first = 0;int second = 0;
                           memcpy(&first, record_to_print_final[j]+orderby_offset, sizeof(int));
                           memcpy(&second,record_to_print_final[j+1]+orderby_offset,sizeof(int));
                            if(!order_desc && first < second ){
                                char *temp = record_to_print_final[j];
                                record_to_print_final[j] = record_to_print_final[j+1];
                                record_to_print_final[j+1] = temp;
                            }
                            else if(order_desc && second < first){
                              char *temp  = record_to_print_final[j];
                              record_to_print_final[j] = record_to_print_final[j+1];
                              record_to_print_final[j+1] = temp;
                            }
                      }
                    }//End INT
                  } //End loop
                }//End order by
                int aggregate_result = 0;
                // PRINT EVERYTHING
                for(int z = 0; z < total_row; z++){
                    record_ptr = record_to_print_final[z];
                    if(select_all) { // SELECT ALL
                       int record_offset2 = 0;
                       for(j = 0; j < tab_entry->num_columns; j++)
                       {
                        /* If the column type is CHAR*/
                        unsigned char tok_length2 = NULL;
                        memcpy(&tok_length2, record_ptr+record_offset2, 1);
                        record_offset2++;

                        printf("%s","|");
                        if(list_cd_entry[j]->col_type == T_CHAR || list_cd_entry[j]->col_type == T_VARCHAR){
                         int col_len2 = list_cd_entry[j]->col_len;
                         if(tok_length2 == 0){ //NULL
                            printf("%*s",-FORMAT_LENGTH, "-");
                         }
                         else {
                           char temp_string[tok_length2 + 1];
                           memset(temp_string, '\0', tok_length2 + 1);
                           memcpy(&temp_string, record_ptr+record_offset2, tok_length2);
                           printf("%*s", -FORMAT_LENGTH ,temp_string);
                         }
                       }
                       else { //column type is INT
                         if(tok_length2 == 0){ //NULL
                           printf("%*s",FORMAT_LENGTH,"-");
                         }
                         else {
                           int value = 0;
                           memcpy(&value, record_ptr+record_offset2, sizeof(int));
                           printf("%*d",FORMAT_LENGTH,value);
                         }
                       }
                       record_offset2 = record_offset2 + list_cd_entry[j]->col_len;
                      }
                    }
                    else if(!select_all) {
                      int record_offset2 = 0;
                      for(int i = 0; i < count_proj; i++){ // for all projection column
                        for(j = 0; j < tab_entry->num_columns; j++) //search through all 5 columns
                        {
                          int tok_length2 = NULL;
                          memcpy(&tok_length2, record_ptr+record_offset2, 1);
                          record_offset2++;
                         /* If the column type is CHAR*/
                         if(strcasecmp(proj_col[i], list_cd_entry[j]->col_name) == 0){
                           printf("%s","|");
                           if(list_cd_entry[j]->col_type == T_CHAR || list_cd_entry[j]->col_type == T_VARCHAR){
                            if(tok_length2 == 0){ //NULL
                               printf("%*s",-FORMAT_LENGTH, "-");
                            }
                            else {
                              char temp_string[tok_length2 + 1];
                              memset(temp_string, '\0', tok_length2 + 1);
                              memcpy(&temp_string, record_ptr+record_offset2, tok_length2);
                              printf("%*s", -FORMAT_LENGTH ,temp_string);
                            }
                          }
                          else { //column type is INT
                            if(tok_length2 == 0){ //NULL
                              printf("%*s",FORMAT_LENGTH,"-");
                            }
                            else {
                              int value = 0;
                              memcpy(&value, record_ptr+record_offset2, sizeof(int));
                              printf("%*d",FORMAT_LENGTH,value);
                            }
                          }
                          i++;
                         }
                         record_offset2 = record_offset2 + list_cd_entry[j]->col_len;
                        }
                     }
                    }
                    printf("|\n");

                }
                int count = 0;
                for(count = 0; count < print_column; count++){
                  printf("%s","+--------------------");
                }
                printf("+\n");
                printf("%d rows selected.\n", total_row);

                fflush(fhandle);
                fclose(fhandle);
              }
          }
          // if(!rc)
          //   {
          //     cur = cur->next; //Should be the terminator
          //     if (cur->tok_value != EOC)
          //       {
          //         rc = INVALID_TABLE_DEFINITION;
          //         cur->tok_value = INVALID;
          //       }//End checking for terminator
          //   }
          }//End checking if that table exist
      }// End checking for invalid table

    return rc;
}
int sem_delete(token_list *t_list) {
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
  char* record_ptr = NULL;
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
      else {
          strcpy(tab_entry.table_name, cur->tok_string);
          strcpy(filename, cur->tok_string);
          strcat(filename, ".tab");
          cur = cur->next;
          char* record_ptr = (char *)load_data_from_tab((char *)tab_entry.table_name);
          tabfile_ptr = (table_file_header*)record_ptr;
          if (cur->tok_value == EOC){ //Delete All
              //Cast to get the table_file_header struc
              tabfile_ptr->file_size = sizeof(table_file_header);
              tabfile_ptr->num_records = 0;
              fhandle = fopen(filename, "w+bc");
              fwrite(tabfile_ptr, sizeof(table_file_header), 1, fhandle);
              fflush(fhandle);
              fclose(fhandle);
          }
          else if(cur->tok_value == K_WHERE){ //Delete with condition
              cur = cur->next;
              //check for valid column name
              bool found_column = false;
              int column_offset = 0;
              int i = 0;
              cd_entry* match_col = NULL;
              //Loop through all the columns
              for(i = 0, col_entry = (cd_entry*)((char*)new_entry + new_entry->cd_offset);
              i < new_entry->num_columns; i++, col_entry++)
              {
                  if(!found_column){
                      //If there is such column
                      if(strcasecmp(cur->tok_string, col_entry->col_name) == 0){
                        found_column = true;
                        match_col = col_entry;
                      }
                      else {
                        column_offset += col_entry->col_len + 1;
                      }
                  }
              }
              if(!found_column){
                  rc = COLUMN_NOT_EXIST;
                  printf("No Column %s Found\n",cur->tok_string);
                  cur->tok_value = INVALID;
              }
              if(!rc)
              {
                cur = cur->next;
                if(cur->tok_value != S_EQUAL && cur->tok_value != S_LESS && cur->tok_value != S_GREATER){
                  rc = INVALID_DELETE_SYNTAX;
                  printf("%s\n", "Invalid syntax in the WHERE clause" );
                  cur->tok_value = INVALID;
                }
                else {
                  int sign = cur->tok_value;
                  cur = cur->next; //moving to data value
                  //Remember to use column_offset
                  char* end_of_file = record_ptr + tabfile_ptr->file_size;
                  int record_offset = tabfile_ptr->record_offset + column_offset;
                  int cur_row = 0;
                  int num_row_changed = 0;
                  int count = 0;
                  for(cur_row = 0; cur_row < tabfile_ptr->num_records; cur_row++){
                      char* eof_offset;
                      if(match_col->col_type == T_CHAR || match_col->col_type == T_VARCHAR) // VARCHAR
                      {
                        char temp_string[match_col->col_len+1];
                        char replace_string[match_col->col_len+1];
                        memset(temp_string,'\0',match_col->col_len+1);
                        memcpy(&temp_string,record_ptr+record_offset+1,match_col->col_len);
                        //Found the matching row
                        if(strcasecmp(cur->tok_string, temp_string) == 0){
                            count++;
                            eof_offset = end_of_file-(count*tabfile_ptr->record_size)+column_offset+1;
                            memset(replace_string,'\0',match_col->col_len+1); //empty the string
                            memcpy(&replace_string,eof_offset,match_col->col_len);
                            while(strcasecmp(cur->tok_string, replace_string) == 0){
                              count++;
                              eof_offset = end_of_file-(count*tabfile_ptr->record_size)+column_offset+1;
                              memset(replace_string,'\0',match_col->col_len+1); //empty the string
                              memcpy(&replace_string,eof_offset,match_col->col_len);
                            }
                            if(strcasecmp(cur->tok_string, replace_string) != 0) {
                              num_row_changed++;
                              memcpy(
                                record_ptr+record_offset-column_offset,
                                end_of_file-(count) * tabfile_ptr->record_size,
                                tabfile_ptr->record_size);
                            }
                        }
                      }
                      else { //must be an int
                        int value = 0;
                        int replace_value = 0;
                        memcpy(&value, record_ptr+record_offset+1, sizeof(int));

                        if(sign == S_EQUAL && value == atoi(cur->tok_string)){
                          count++;
                          eof_offset = end_of_file-(count*tabfile_ptr->record_size)+column_offset+1;
                          memcpy(&replace_value, eof_offset, sizeof(int));

                          while(replace_value == atoi(cur->tok_string)){
                            count++;
                            eof_offset = eof_offset-tabfile_ptr->record_size;
                            memcpy(&replace_value, eof_offset, sizeof(int));
                          }
                          if(replace_value != atoi(cur->tok_string)){
                            num_row_changed++;
                            memcpy(
                              record_ptr+record_offset-column_offset,
                              end_of_file-(count) * tabfile_ptr->record_size,
                              tabfile_ptr->record_size);
                          }
                        }
                        if(sign == S_LESS && value < atoi(cur->tok_string)){
                          count++;
                          eof_offset = end_of_file-(count*tabfile_ptr->record_size)+column_offset+1;
                          memcpy(&replace_value, eof_offset, sizeof(int));

                          while(replace_value < atoi(cur->tok_string)){
                            count++;
                            eof_offset = eof_offset-tabfile_ptr->record_size;
                            memcpy(&replace_value, eof_offset, sizeof(int));
                          }
                          if(replace_value >= atoi(cur->tok_string)){
                            num_row_changed++;
                            memcpy(
                              record_ptr+record_offset-column_offset,
                              end_of_file-(count) * tabfile_ptr->record_size,
                              tabfile_ptr->record_size);
                          }
                        }
                        if(sign == S_GREATER && value > atoi(cur->tok_string)){
                          count++;
                          eof_offset = end_of_file-(count*tabfile_ptr->record_size)+column_offset+1;
                          memcpy(&replace_value, eof_offset, sizeof(int));

                          while(replace_value > atoi(cur->tok_string)){
                            count++;
                            eof_offset = eof_offset-tabfile_ptr->record_size;
                            memcpy(&replace_value, eof_offset, sizeof(int));
                          }
                          if(replace_value <= atoi(cur->tok_string)){
                            num_row_changed++;
                            memcpy(
                              record_ptr+record_offset-column_offset,
                              end_of_file-(count) * tabfile_ptr->record_size,
                              tabfile_ptr->record_size);
                          }
                        }
                      }
                      record_offset += tabfile_ptr->record_size;
                  }
                  if(num_row_changed > 0){
                      printf("Delete %d rows. \n", num_row_changed);
                      tabfile_ptr->num_records = tabfile_ptr->num_records - num_row_changed;
                      tabfile_ptr->file_size = tabfile_ptr->file_size - (tabfile_ptr->record_size * num_row_changed);
                      fhandle = fopen(filename,"wbc");
                      fwrite(record_ptr, tabfile_ptr->file_size, 1, fhandle);
                  }
                  else {
                    printf("Delete %d rows. \n", num_row_changed);
                  }
                  fflush(fhandle);
                  fclose(fhandle);
                } // End change value
              }//Start parsing more
          }//End WHERE clause
          else {
             rc = INVALID_DELETE_SYNTAX;
             printf("%s\n", "Invalid syntax" );
             cur->tok_value = INVALID;
          }
      }
  }
  return rc;
}

int sem_update(token_list *t_list) {
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
  char* record_ptr = NULL;
  char filename[MAX_IDENT_LEN+5];

  /* Set the current pointer to the token list */
  cur = t_list;
  /* Check for table name*/
  if ((cur->tok_class != keyword) &&
      (cur->tok_class != identifier) &&
      (cur->tok_class != type_name))
  {
      rc = INVALID_TABLE_NAME;
      cur->tok_value = INVALID;
  }
  else {  /* A valid keyword */
      if ((new_entry = get_tpd_from_list(cur->tok_string)) == NULL){
          rc = TABLE_NOT_EXIST;
          cur->tok_value = INVALID;
      }
      else  /* If the table exists  */
      {
          strcpy(tab_entry.table_name, cur->tok_string);
          strcpy(filename, cur->tok_string);
          strcat(filename, ".tab");

          cur = cur->next;
          /* check the token is SET */
          if (cur->tok_value != K_SET)
          {
              rc = INVALID_UPDATE_SYNTAX;
              printf("%s\n", "Keyword SET is missing" );
              cur->tok_value = INVALID;
          }
          else{
              //Supposed to be column
              cur = cur->next;
              //Keep track of offset where buffer start
              int buffer_offset = 0;
              char* record_ptr = (char *)load_data_from_tab((char *)tab_entry.table_name);
              //Cast to get the table_file_header struc
              tabfile_ptr = (table_file_header*)record_ptr;
              cd_entry *match_col = NULL;
              int column_offset = 0;

              bool found_column = false;
              int i = 0;

              cd_entry *match_col_update = NULL;
              bool found_column_update = true;
              int column_offset_update = 0;
              bool where_flag = false;
              token_list* cur2 = cur->next->next->next; //Should be at column_name
              if(cur2->tok_value == K_WHERE) {
                // printf("WHERE ACCEPTED \n");
                cur2 = cur2->next;
                found_column_update = false;
                where_flag = true;
              }
              //Loop through all the columns
              for(i = 0, col_entry = (cd_entry*)((char*)new_entry + new_entry->cd_offset);
              i < new_entry->num_columns; i++, col_entry++)
              {
                  if(!found_column){
                      //If there is such column
                      if(strcasecmp(cur->tok_string, col_entry->col_name) == 0){
                        found_column = true;
                        match_col = col_entry;
                      }
                      else {
                        column_offset += col_entry->col_len + 1;
                      }
                  }

                  if(!found_column_update){
                    if(strcasecmp(cur2->tok_string, col_entry->col_name) == 0){
                      found_column_update = true;
                      match_col_update = col_entry;
                    }
                    else {
                      column_offset_update += col_entry->col_len + 1;
                    }
                  }
              }
              if(!found_column){
                rc = UPDATE_NO_COLUMN;
                printf("No column %s in this table\n", cur->tok_string);
                cur->tok_value = INVALID;
              }
              if(!found_column_update){
                rc = UPDATE_NO_COLUMN;
                printf("No column %s in this table\n", cur2->tok_string);
                cur2->tok_value = INVALID;
              }
              //Check for sign for ints
              if(!rc){
                cur = cur->next;
                if(cur->tok_value != S_EQUAL && cur->tok_value != S_LESS && cur->tok_value != S_GREATER )
                {
                  rc = INVALID_UPDATE_SYNTAX;
                  printf("%s\n", " = is missing" );
                  cur->tok_value = INVALID;
                }
                else { // check for data value
                    cur = cur->next;
                    if(!rc){ // UPDATE ALL AND where
                        int num_row_changed = 0;
                        int ptr_offset = tabfile_ptr->record_offset;
                        int cur_row = 0;
                        int rel_op = 0;
                        if(where_flag){
                            rel_op = cur2->next->tok_value; //relational operator
                            cur2 = cur2->next->next; // move to the comparable value
                        }
                        for(cur_row = 0; cur_row < tabfile_ptr->num_records; cur_row++){
                            if(where_flag){//UPDATE WHERE
                              char* read_pointer = record_ptr+ptr_offset+column_offset_update+1;
                              char* write_pointer = record_ptr + ptr_offset + column_offset;
                              int read_length = match_col_update->col_len;
                              int write_length = match_col->col_len + 1;
                              int column_type = match_col_update->col_type;
                              if(column_type == T_CHAR || column_type == T_VARCHAR) // VARCHAR
                              {
                                char temp_string[read_length + 1];
                                memset(temp_string,'\0', read_length + 1);
                                memcpy(&temp_string,read_pointer,read_length);
                                if(strcasecmp(cur2->tok_string, temp_string) == 0){
                                  num_row_changed++;
                                  if(cur->tok_value == K_NULL){ //NULL
                                    if(match_col->not_null == 1){
                                      rc = INSERT_NOT_NULL_EXCEPTION;
                                      cur->tok_value = INVALID;
                                      printf("%s%s\n", "Not Null constraint exists for column name ", match_col->col_name );

                                    }else{
                                      memset(write_pointer, '\0', write_length);
                                    }
                                  }
                                  else if(cur->tok_value == STRING_LITERAL){ //STRING
                                    if(match_col->col_type != T_CHAR && match_col->col_type != T_VARCHAR){
                                      rc = INVALID_UPDATE_DATATYPE;
                                      printf("%s\n", "Type mismatch at SET");
                                      cur->tok_value = INVALID;
                                    }
                                    else {
                                      int tok_length = strlen(cur->tok_string);
                                      unsigned char temp_len_chr = tok_length;
                                      unsigned char *p_len = &temp_len_chr;
                                      memcpy(write_pointer, p_len, 1);
                                      memcpy(write_pointer + 1, cur->tok_string, match_col->col_len);
                                    }
                                  }
                                  else if(cur->tok_value == INT_LITERAL) { //INT
                                    if(match_col->col_type != T_INT){
                                      rc = INVALID_UPDATE_DATATYPE;
                                      printf("%s\n", "Type mismatch at SET");
                                      cur->tok_value = INVALID;
                                    }
                                    else {
                                      int tok_length = sizeof(int);
                                      unsigned char temp_len_chr = tok_length;
                                      unsigned char *p_len = &temp_len_chr;
                                      memcpy(write_pointer, p_len, 1);

                                      int temp_int = atoi(cur->tok_string);
                                      int *p_int = &temp_int;
                                      memcpy(write_pointer + 1, p_int, match_col->col_len );
                                    }
                                  }
                                } // End compare String
                              }
                              else { //Int - need to process sign
                                int temp_num = 0;
                                memcpy(&temp_num, read_pointer, sizeof(int));
                                if( (temp_num == atoi(cur2->tok_string) && rel_op == S_EQUAL)
                                  || (temp_num < atoi(cur2->tok_string) && rel_op == S_LESS)
                                  || (temp_num > atoi(cur2->tok_string) && rel_op == S_GREATER)){
                                  num_row_changed++;
                                  if(cur->tok_value == K_NULL){ //NULL
                                    if(match_col->not_null == 1){
                                      rc = INSERT_NOT_NULL_EXCEPTION;
                                      cur->tok_value = INVALID;
                                      printf("%s%s\n", "Not Null constraint exists for column name ", match_col->col_name );

                                    }else{
                                      memset(write_pointer, '\0', write_length);
                                    }
                                  }
                                  else if(cur->tok_value == STRING_LITERAL){ //STRING
                                    if(match_col->col_type != T_CHAR && match_col->col_type != T_VARCHAR){
                                      rc = INVALID_UPDATE_DATATYPE;
                                      printf("%s\n", "Type mismatch at SET");
                                      cur->tok_value = INVALID;
                                    }
                                    else {
                                      int tok_length = strlen(cur->tok_string);
                                      unsigned char temp_len_chr = tok_length;
                                      unsigned char *p_len = &temp_len_chr;
                                      memcpy(record_ptr + ptr_offset + column_offset, p_len, 1);
                                      memcpy(record_ptr + ptr_offset + column_offset + 1, cur->tok_string, match_col->col_len);
                                    }
                                  }
                                  else if(cur->tok_value == INT_LITERAL) { //INT
                                    if(match_col->col_type != T_INT){
                                      rc = INVALID_UPDATE_DATATYPE;
                                      printf("%s\n", "Type mismatch at SET");
                                      cur->tok_value = INVALID;
                                    }
                                    else {
                                      int tok_length = sizeof(int);
                                      unsigned char temp_len_chr = tok_length;
                                      unsigned char *p_len = &temp_len_chr;
                                      memcpy(write_pointer, p_len, 1);

                                      int temp_int = atoi(cur->tok_string);
                                      int *p_int = &temp_int;
                                      memcpy(write_pointer + 1, p_int, match_col->col_len );
                                    }
                                  }
                                }//End compare Int
                              }
                            }
                            else { //UPDATE ALL
                              num_row_changed++;
                              if(cur->tok_value == K_NULL){ //NULL
                                if(match_col->not_null == 1){
                                  rc = INSERT_NOT_NULL_EXCEPTION;
                                  cur->tok_value = INVALID;
                                  printf("%s%s\n", "Not Null constraint exists for column name ", match_col->col_name );

                                }else{
                                  memset(record_ptr + ptr_offset + column_offset,'\0',match_col->col_len + 1);                                }
                              }
                              else if(cur->tok_value == STRING_LITERAL){ //STRING
                                if(match_col->col_type != T_CHAR && match_col->col_type != T_VARCHAR){
                                  rc = INVALID_UPDATE_DATATYPE;
                                  printf("%s\n", "Type mismatch at SET");
                                  cur->tok_value = INVALID;
                                }
                                else {
                                  int tok_length = strlen(cur->tok_string);
                                  unsigned char temp_len_chr = tok_length;
                                  unsigned char *p_len = &temp_len_chr;
                                  memcpy(record_ptr + ptr_offset + column_offset, p_len, 1);
                                  memcpy(record_ptr + ptr_offset + column_offset + 1, cur->tok_string, match_col->col_len);
                                }
                              }
                              else if(cur->tok_value == INT_LITERAL) { //INT
                                if(match_col->col_type != T_INT){
                                  rc = INVALID_UPDATE_DATATYPE;
                                  printf("%s\n", "Type mismatch at SET");
                                  cur->tok_value = INVALID;
                                }
                                else {
                                   int tok_length = sizeof(int);
                                   unsigned char temp_len_chr = tok_length;
                                   unsigned char *p_len = &temp_len_chr;
                                   memcpy(record_ptr + ptr_offset + column_offset, p_len, 1);

                                   int temp_int = atoi(cur->tok_string);
                                   int *p_int = &temp_int;
                                   memcpy(record_ptr + ptr_offset + column_offset + 1, p_int, match_col->col_len );
                                }
                              }
                            }
                            ptr_offset += tabfile_ptr->record_size;
                        }
                        if(!rc){
                          printf("Updated %d rows.\n", num_row_changed);
                          fhandle = fopen(filename,"r+bc");
                          fwrite(record_ptr, tabfile_ptr->file_size, 1, fhandle);
                          fflush(fhandle);
                          fclose(fhandle);
                        }
                        //Updating all
                    }
                }//end checking for data value

              }//at equal sign
            }
          }//Check column
      }  //Check if table exists
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
