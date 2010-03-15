/*
 * This library is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation; either version 2.1 of
 * the License.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 *
 * (c) Copyright 2006-2009 Jari OS Core Team <http://jarios.org>
 * (c) Copyright 2009 Stanislav M. Ivankin <grepz@jarios.org>
 *
 * Josconf is an upper level functions for libsexpr, which makes parsing
 * of conffiles easier
 *
 */

#include <stdlib.h>
#include <string.h>

#include <josconf.h>

sexp_syntax_err_t sexp_get_value(sexp_t *expr, atom_t type,
				 char *data, unsigned int maxlen)
{
  if (!expr)
    return SEXP_SYNTAX_VAL_ERR;
  if (!SEXP_IS_TYPE(expr, type))
    return SEXP_SYNTAX_TYPE_ERR;
  if (maxlen && strlen(expr->val) > maxlen)
    return SEXP_SYNTAX_LENGTH_ERR;

  strcpy(data, expr->val);

  return SEXP_SYNTAX_OK;
}

sexp_syntax_err_t sexp_get_value_alloc(sexp_t *expr, atom_t type,
				       char **data)
{
  char *data_ptr;
  int len;

  if (!expr) return SEXP_SYNTAX_VAL_ERR;
  if (!SEXP_IS_TYPE(expr, type))
    return SEXP_SYNTAX_TYPE_ERR;

  len = strlen(expr->val) + 1;

  data_ptr = malloc(len);
  if (!data_ptr)
    return SEXP_SYNTAX_MEM_ERR;

  memcpy(data_ptr, expr->val, len);

  *data = data_ptr;

  return SEXP_SYNTAX_OK;
}

sexp_syntax_err_t sexp_get_key_sx(sexp_t *expr, const char *key,
                                  sexp_t **key_sx)
{
  sexp_t *sx;

  // TODO: Add key char later to key name
  //  if (!expr || strchr(key, ':'))
  if (!expr || (*key != ':'))
    return SEXP_SYNTAX_VAL_ERR;

  sx = find_sexp(key, expr);
  if (!sx)
    return SEXP_SYNTAX_NOTFOUND_ERR;

  if (!SEXP_IS_TYPE(sx, SEXP_BASIC) || !SEXP_IS_KEY(sx))
    return SEXP_SYNTAX_TYPE_ERR;

  *key_sx = sx->next;

  return SEXP_SYNTAX_OK;
}

sexp_syntax_err_t sexp_get_key_value(sexp_t *expr, atom_t type,
				     const char *key, char *data,
				     unsigned int maxlen)
{
  sexp_t *key_sx = NULL;
  sexp_syntax_err_t err;

  err = sexp_get_key_sx(expr, key, &key_sx);
  if (err)
    return err;

  if (!key_sx || !SEXP_IS_TYPE(key_sx, type))
    return SEXP_SYNTAX_VAL_ERR;

  if (maxlen && (strlen(key_sx->val) > maxlen))
    return SEXP_SYNTAX_LENGTH_ERR;

  strcpy(data, key_sx->val);

  return SEXP_SYNTAX_OK;
}

sexp_syntax_err_t sexp_get_key_value_alloc(sexp_t *expr, atom_t type,
                                           const char *key, char **data,
                                           unsigned int maxlen)
{
  char *data_ptr = NULL;
  sexp_t *key_sx = NULL;
  sexp_syntax_err_t err;
  int len;

  err = sexp_get_key_sx(expr, key, &key_sx);
  if (err)
    return err;

  if (!key_sx || !SEXP_IS_TYPE(key_sx, type))
    return SEXP_SYNTAX_VAL_ERR;

  len = strlen(key_sx->val) + 1;
  if (maxlen != 0 && (len > maxlen))
    return SEXP_SYNTAX_VAL_ERR;

  data_ptr = malloc(len);
  if (!data_ptr)
    return SEXP_SYNTAX_MEM_ERR;

  strcpy(data_ptr, key_sx->val);
  //memcpy(data_ptr, key_sx->val, len);
  *data = data_ptr;

  return SEXP_SYNTAX_OK;
}

sexp_syntax_err_t sexp_list_car(sexp_t *expr, sexp_t **sx)
{
  if (!SEXP_IS_LIST(expr) || expr->list->ty != SEXP_VALUE)
    return SEXP_SYNTAX_TYPE_ERR;

  *sx = expr->list;

  return SEXP_SYNTAX_OK;
}

sexp_syntax_err_t sexp_list_cdr(sexp_t *expr, sexp_t **sx)
{
  /* Dummy function. Can we do cdr properly? */
  if (!SEXP_IS_LIST(expr) || expr->list->ty != SEXP_VALUE)
    return SEXP_SYNTAX_TYPE_ERR;

  if (!expr->list->next)
    *sx = NULL;
  else
    *sx = expr->list->next;

  return SEXP_SYNTAX_OK;
}

sexp_syntax_err_t sexp_value_by_name(sexp_t *expr, atom_t type,
				     const char *token_name,
				     char *data,
				     unsigned int maxlen)
{
  int ret;
  sexp_t *token;

  token = find_sexp(token_name, expr);
  if (!token)
    return SEXP_SYNTAX_NOTFOUND_ERR;

  ret = sexp_get_value(token, type, data, maxlen);
  if (ret)
    return ret;

  return SEXP_SYNTAX_OK;
}

sexp_syntax_err_t sexp_list_get_element(sexp_t *expr, sexp_t **sx, int n)
{
  int length, i;
  sexp_t *ptr;

  if (!SEXP_IS_LIST(expr) || expr->list->ty != SEXP_VALUE)
    return SEXP_SYNTAX_TYPE_ERR;

  length = sexp_list_length(expr);
  if (n >= length)
    return SEXP_SYNTAX_VAL_ERR;

  ptr = expr->list;
  for (i = 0; i < n; i++)
    ptr = ptr->next;

  *sx = ptr;

  return SEXP_SYNTAX_OK;
}

int sexp_list_length_ext(const sexp_t *sx)
{
  int len = 0;
  const sexp_t *ptr;

  if (sx == NULL)
    return 0;

  ptr = (sx->ty == SEXP_VALUE) ? sx : sx->list;

  for (;ptr; ptr = ptr->next)
    len ++;

  return len;
}
