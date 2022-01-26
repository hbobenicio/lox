#include "ast-printer.h"

#include "expr.h"
#include <stdbool.h>
#include <assert.h>

struct ast_printer {
    FILE* file;
};

static void ast_printer_visit_binary(struct expr_binary* expr_bin, void* userctx);
static void ast_printer_visit_grouping(struct expr_grouping* expr_group, void* userctx);
static void ast_printer_visit_literal(struct expr_literal* expr_lit, void* userctx);
static void ast_printer_visit_unary(struct expr_unary* expr_un, void* userctx);

static const struct expr_visitor ast_printer_expr_visitor = {
    .visit_binary = ast_printer_visit_binary,
    .visit_grouping = ast_printer_visit_grouping,
    .visit_literal = ast_printer_visit_literal,
    .visit_unary = ast_printer_visit_unary,
};

void ast_printer_println(struct expr* expr) {
    ast_printer_fprintln(stdout, expr);
}

void ast_printer_fprintln(FILE* file, struct expr* expr) {
    struct ast_printer ast_printer = {
        .file = file,
    };

    expr_accept(expr, &ast_printer_expr_visitor, &ast_printer);

    fputs("\n", file);
}

static void ast_printer_visit_binary(struct expr_binary* expr_bin, void* userctx) {
    struct ast_printer* ast_printer = userctx;

    char op = expr_bin->operator.lexeme.ptr[0];

    fprintf(ast_printer->file, "(%c ", op);
    expr_accept(expr_bin->left, &ast_printer_expr_visitor, userctx);
    fprintf(ast_printer->file, " ");
    expr_accept(expr_bin->right, &ast_printer_expr_visitor, userctx);
    fprintf(ast_printer->file, ")");
}

static void ast_printer_visit_grouping(struct expr_grouping* expr_group, void* userctx) {
    struct ast_printer* ast_printer = userctx;

    fprintf(ast_printer->file, "(group ");
    expr_accept(expr_group->expr, &ast_printer_expr_visitor, userctx);
    fprintf(ast_printer->file, ")");
}

static void ast_printer_visit_literal(struct expr_literal* expr_lit, void* userctx) {
    struct ast_printer* ast_printer = userctx;
    
    switch (expr_lit->kind) {
    case EXPR_LITERAL_KIND_NUMBER:
        fprintf(ast_printer->file, "%lf", expr_lit->value.number.val);
        break;

    case EXPR_LITERAL_KIND_STRING:
        fprintf(ast_printer->file, "%s", expr_lit->value.string.val.ptr);
        break;

    case EXPR_LITERAL_KIND_BOOL:
        fprintf(ast_printer->file, "%s", (expr_lit->value.boolean.val) ? "true" : "false");
        break;

    case EXPR_LITERAL_KIND_NIL:
        fputs("nil", ast_printer->file);
        break;

    default:
        assert(false && "unsupported expr_lit variant kind. did you added a new variant to it recently?");
        break;
    }
}

static void ast_printer_visit_unary(struct expr_unary* expr_un, void* userctx) {
    struct ast_printer* ast_printer = userctx;
    
    char op = expr_un->operator.lexeme.ptr[0];
    fprintf(ast_printer->file, "(%c ", op);
    expr_accept(expr_un->right, &ast_printer_expr_visitor, userctx);
    fprintf(ast_printer->file, ")");
}