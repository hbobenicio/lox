#include "ast-printer.h"

#include <stdbool.h>
#include <assert.h>

#include "expr.h"
#include "expr-visitor.h"

struct ast_printer {
    FILE* file;
};

static void ast_printer_visit_binary(struct clox_ast_expr* expr, void* userctx);
static void ast_printer_visit_grouping(struct clox_ast_expr* expr, void* userctx);
static void ast_printer_visit_literal(struct clox_ast_expr* expr, void* userctx);
static void ast_printer_visit_unary(struct clox_ast_expr* expr, void* userctx);

static const struct clox_ast_expr_visitor ast_printer_expr_visitor = {
    .visit_binary = ast_printer_visit_binary,
    .visit_grouping = ast_printer_visit_grouping,
    .visit_literal = ast_printer_visit_literal,
    .visit_unary = ast_printer_visit_unary,
};

void ast_printer_println(struct clox_ast_expr* expr) {
    ast_printer_fprintln(stdout, expr);
}

void ast_printer_fprintln(FILE* file, struct clox_ast_expr* expr) {
    struct ast_printer ast_printer = {
        .file = file,
    };

    clox_ast_expr_accept(expr, &ast_printer_expr_visitor, &ast_printer);

    fputs("\n", file);
}

static void ast_printer_visit_binary(struct clox_ast_expr* expr, void* userctx) {
    struct ast_printer* ast_printer = userctx;
    struct clox_ast_expr_binary* expr_bin = &expr->value.binary;

    char op = expr_bin->operator.lexeme.ptr[0];

    fprintf(ast_printer->file, "(%c ", op);
    clox_ast_expr_accept(expr_bin->left, &ast_printer_expr_visitor, userctx);
    fprintf(ast_printer->file, " ");
    clox_ast_expr_accept(expr_bin->right, &ast_printer_expr_visitor, userctx);
    fprintf(ast_printer->file, ")");
}

static void ast_printer_visit_grouping(struct clox_ast_expr* expr, void* userctx) {
    struct ast_printer* ast_printer = userctx;
    struct clox_ast_expr_grouping* expr_group = &expr->value.grouping;

    fprintf(ast_printer->file, "(group ");
    clox_ast_expr_accept(expr_group->expr, &ast_printer_expr_visitor, userctx);
    fprintf(ast_printer->file, ")");
}

static void ast_printer_visit_literal(struct clox_ast_expr* expr, void* userctx) {
    struct ast_printer* ast_printer = userctx;
    struct clox_ast_expr_literal* expr_lit = &expr->value.literal;
    
    switch (expr_lit->kind) {
    case CLOX_AST_EXPR_LITERAL_KIND_NUMBER:
        fprintf(ast_printer->file, "%lf", expr_lit->value.number.val);
        break;

    case CLOX_AST_EXPR_LITERAL_KIND_STRING:
        fprintf(ast_printer->file, "%s", expr_lit->value.string.val.ptr);
        break;

    case CLOX_AST_EXPR_LITERAL_KIND_BOOL:
        fprintf(ast_printer->file, "%s", (expr_lit->value.boolean.val) ? "true" : "false");
        break;

    case CLOX_AST_EXPR_LITERAL_KIND_NIL:
        fputs("nil", ast_printer->file);
        break;
    }
}

static void ast_printer_visit_unary(struct clox_ast_expr* expr, void* userctx) {
    struct ast_printer* ast_printer = userctx;
    struct clox_ast_expr_unary* expr_un = &expr->value.unary;
    
    char op = expr_un->operator.lexeme.ptr[0];
    fprintf(ast_printer->file, "(%c ", op);
    clox_ast_expr_accept(expr_un->right, &ast_printer_expr_visitor, userctx);
    fprintf(ast_printer->file, ")");
}