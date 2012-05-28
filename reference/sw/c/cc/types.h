/*
    Copyright 2011 the PLP authors

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

 */

#ifndef TYPES
#define TYPES

#define ATTR_STRUCT     0x00000001
#define ATTR_ENUM       0x00000002
#define ATTR_AUTO       0x00000004
#define ATTR_REGISTER   0x00000008
#define ATTR_STATIC     0x00000010
#define ATTR_EXTERN     0x00000020
#define ATTR_TYPEDEF    0x00000040
#define ATTR_UNSIGNED   0x00000080
#define ATTR_SIGNED     0x00000100
#define ATTR_VOLATILE   0x00000200
#define ATTR_FUNCTION   0x00000400
#define ATTR_POINTER    0x00000800
#define ATTR_PARAM	0x00001000

/* symbol table types */
typedef struct symbol_t {
        unsigned int            attr;
        char *                  type;
        char *                  value;
	int 			size;
        struct symbol_t *       up;
} symbol;

typedef struct symbol_table_t {
        struct symbol_table_t *parent;
        symbol *s;
        symbol *assoc;
        int num_children;
        struct symbol_table_t **children;
} symbol_table;

/* struct element */
typedef struct struct_table_t {
	struct struct_table_t *next;
	char *name;
	symbol_table *s;
} struct_table;

/* node types */
typedef enum {
        type_con,
        type_id,
        type_op,
        type_type,
        type_string
} node_type;

typedef struct node_t {
        node_type type;                 /* node type */
        char *id;                       /* the node id field, contains a constant if type_con, variable name if type_id, a keyword if type_op */
        symbol_table* t;                /* the symbol table (scope) associated with this node */
	int line;			/* line number in the source that this node represents */
	int column;			/* column that this node represents */
        int num_children;               /* number of values in the next field */
        struct node_t *children[];      /* an array of child nodes */
} node;
#endif
