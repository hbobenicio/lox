#ifndef CLOX_EXPR_H
#define CLOX_EXPR_H

#include <stdbool.h>
#include "token.h"
#include "str.h"
#include "strview.h"
struct expr {
    enum expr_kind {
        EXPR_KIND_BINARY,
        EXPR_KIND_GROUPING,
        EXPR_KIND_LITERAL,
        EXPR_KIND_UNARY,
    } kind;

    union {
        struct expr_binary {
            struct expr* left;
            struct token operator;
            struct expr* right;
        } binary;

        struct expr_grouping {
            struct expr* expr;
        } grouping;

        struct expr_literal {
            enum expr_literal_kind {
                EXPR_LITERAL_KIND_NUMBER,
                EXPR_LITERAL_KIND_STRING,
                EXPR_LITERAL_KIND_BOOL,
                EXPR_LITERAL_KIND_NIL,
            } kind;

            union {
                struct expr_literal_number {
                    double val;
                } number;

                struct expr_literal_string {
                    // TODO this could be improved maybe... it's ok for now
                    struct str val;
                } string;

                struct expr_literal_bool {
                    bool val;
                } boolean;
            } value;
        } literal;

        struct expr_unary {
            struct token operator;
            struct expr* right;
        } unary;

    } value;
};

struct expr* expr_binary_new(struct expr* left, struct token operator, struct expr* right);
struct expr* expr_unary_new(struct token operator, struct expr* right);
struct expr* expr_literal_bool_new(bool val);
struct expr* expr_literal_nil_new(void);
struct expr* expr_literal_string_new(struct strview sv);
struct expr* expr_literal_number_new(double val);
struct expr* expr_grouping_new(struct expr* expr);
struct expr  expr_literal_number_create(double val);
struct expr  expr_grouping_create(struct expr* expr);

struct expr_visitor {
    void (*visit_binary)(struct expr_binary* expr_bin, void* userctx);
    void (*visit_grouping)(struct expr_grouping* expr_group, void* userctx);
    void (*visit_literal)(struct expr_literal* expr_lit, void* userctx);
    void (*visit_unary)(struct expr_unary* expr_un, void* userctx);
};

void expr_accept(struct expr* expr, const struct expr_visitor* visitor, void* userctx);

#endif