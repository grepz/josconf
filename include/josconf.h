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


#ifndef JOSCONF_H
#define JOSCONF_H

#ifdef SYSLIB_BUILD
  #include <sexp.h>
  #include <sexp_ops.h>
#endif

#define SEXP_IS_LIST(sx)			\
  ((sx)->ty == SEXP_LIST) ? 1 : 0

#define SEXP_IS_TYPE(sx,type)					\
  ((sx)->ty == SEXP_VALUE && (sx)->aty == (type)) ? 1 : 0

#define SEXP_IS_KEY(sx)						\
  (SEXP_IS_TYPE((sx),SEXP_BASIC) && (*sx->val == ':')) ? 1 : 0

#define SEXP_ITERATE_LIST(lst,iter,ind)                \
  for((ind) = 0, (iter) = (lst)->list;                 \
      ind < sexp_list_length(lst);                     \
      (ind)++, (iter) = (iter)->next)

#define SEXP_ITERATE_LIST_EXT(lst,iter,ind)                             \
  for((ind) = 0, (iter) = (((lst)->ty==SEXP_VALUE)?(lst):(lst)->list);  \
      ind < sexp_list_length_ext(lst);                                  \
      (ind)++, (iter) = (iter)->next)

typedef enum {
  SEXP_SYNTAX_OK = 0,
  SEXP_SYNTAX_TYPE_ERR,
  SEXP_SYNTAX_INCOMPLETE_ERR,
  SEXP_SYNTAX_NOTFOUND_ERR,
  SEXP_SYNTAX_LENGTH_ERR,
  SEXP_SYNTAX_VAL_ERR,
  SEXP_SYNTAX_MEM_ERR,
  SEXP_SYNTAX_UNKNOWN_ERR,
} sexp_syntax_err_t;

/* Get value associated with current sexp_t. data must be already allocated. */
sexp_syntax_err_t sexp_get_value(sexp_t *expr, atom_t type, char *data,
				 unsigned int maxlen);

/* Get value associated with current sexp_t. data is allocated with
 * respect to value is sexp_t.
 */
sexp_syntax_err_t sexp_get_value_alloc(sexp_t *expr, atom_t type, char **data)
  __attribute__ ((warn_unused_result));


/* Get sexp_t object accosiated with key */
sexp_syntax_err_t sexp_get_key_sx(sexp_t *expr, const char *key,
                                  sexp_t **key_sx);

/* Get key value, for example: (:a 1 :b 2 :c 3). Getting key value :a will
 * result in 1
 */
sexp_syntax_err_t sexp_get_key_value(sexp_t *expr, atom_t type, const char *key,
				     char *data, unsigned int maxlen);
sexp_syntax_err_t sexp_get_key_value_alloc(sexp_t *expr, atom_t type,
                                           const char *key, char **data,
                                           unsigned int maxlen);

/* Get nth element of a list */
sexp_syntax_err_t sexp_list_get_element(sexp_t *expr, sexp_t **sx, int n);

/* Get first element of a list */
sexp_syntax_err_t sexp_list_car(sexp_t *expr, sexp_t **sx);

/* It's not really a cdr, it only returns second element of a list
 * TODO: Find proper way of getting cdr of a list.
 */
sexp_syntax_err_t sexp_list_cdr(sexp_t *expr, sexp_t **sx);

/* Get value associated with sexp_t with value token_name */
sexp_syntax_err_t sexp_value_by_name(sexp_t *expr, atom_t type,
				     const char *token_name, char *data,
				     unsigned int maxlen);

/* Extended list length counter, can start from anywhere of list */
int sexp_list_length_ext(const sexp_t *sx);

#endif
