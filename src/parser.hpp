/*
 * Copyright (c) 2015 Andrew Kelley
 *
 * This file is part of zig, which is MIT licensed.
 * See http://opensource.org/licenses/MIT
 */

#ifndef ZIG_PARSER_HPP
#define ZIG_PARSER_HPP

#include "list.hpp"
#include "buffer.hpp"
#include "tokenizer.hpp"
#include "errmsg.hpp"

struct AstNode;
struct CodeGenNode;
struct ImportTableEntry;
struct AsmToken;

enum NodeType {
    NodeTypeRoot,
    NodeTypeRootExportDecl,
    NodeTypeFnProto,
    NodeTypeFnDef,
    NodeTypeFnDecl,
    NodeTypeParamDecl,
    NodeTypeType,
    NodeTypeBlock,
    NodeTypeExternBlock,
    NodeTypeDirective,
    NodeTypeReturnExpr,
    NodeTypeVariableDeclaration,
    NodeTypeBinOpExpr,
    NodeTypeCastExpr,
    NodeTypeNumberLiteral,
    NodeTypeStringLiteral,
    NodeTypeUnreachable,
    NodeTypeSymbol,
    NodeTypePrefixOpExpr,
    NodeTypeFnCallExpr,
    NodeTypeArrayAccessExpr,
    NodeTypeUse,
    NodeTypeVoid,
    NodeTypeBoolLiteral,
    NodeTypeIfExpr,
    NodeTypeLabel,
    NodeTypeGoto,
    NodeTypeAsmExpr,
};

struct AstNodeRoot {
    ZigList<AstNode *> top_level_decls;
};

enum FnProtoVisibMod {
    FnProtoVisibModPrivate,
    FnProtoVisibModPub,
    FnProtoVisibModExport,
};

struct AstNodeFnProto {
    ZigList<AstNode *> *directives;
    FnProtoVisibMod visib_mod;
    Buf name;
    ZigList<AstNode *> params;
    AstNode *return_type;
    bool is_var_args;
};

struct AstNodeFnDef {
    AstNode *fn_proto;
    AstNode *body;
};

struct AstNodeFnDecl {
    AstNode *fn_proto;
};

struct AstNodeParamDecl {
    Buf name;
    AstNode *type;
};

enum AstNodeTypeType {
    AstNodeTypeTypePrimitive,
    AstNodeTypeTypePointer,
    AstNodeTypeTypeArray,
};

struct AstNodeType {
    AstNodeTypeType type;
    Buf primitive_name;
    AstNode *child_type;
    AstNode *array_size;
    bool is_const;
};

struct AstNodeBlock {
    ZigList<AstNode *> statements;
};

struct AstNodeReturnExpr {
    // might be null in case of return void;
    AstNode *expr;
};

struct AstNodeVariableDeclaration {
    Buf symbol;
    bool is_const;
    // one or both of type and expr will be non null
    AstNode *type;
    AstNode *expr;
};

enum BinOpType {
    BinOpTypeInvalid,
    BinOpTypeAssign,
    BinOpTypeBoolOr,
    BinOpTypeBoolAnd,
    BinOpTypeCmpEq,
    BinOpTypeCmpNotEq,
    BinOpTypeCmpLessThan,
    BinOpTypeCmpGreaterThan,
    BinOpTypeCmpLessOrEq,
    BinOpTypeCmpGreaterOrEq,
    BinOpTypeBinOr,
    BinOpTypeBinXor,
    BinOpTypeBinAnd,
    BinOpTypeBitShiftLeft,
    BinOpTypeBitShiftRight,
    BinOpTypeAdd,
    BinOpTypeSub,
    BinOpTypeMult,
    BinOpTypeDiv,
    BinOpTypeMod,
};

struct AstNodeBinOpExpr {
    AstNode *op1;
    BinOpType bin_op;
    AstNode *op2;
};

struct AstNodeFnCallExpr {
    AstNode *fn_ref_expr;
    ZigList<AstNode *> params;
};

struct AstNodeArrayAccessExpr {
    AstNode *array_ref_expr;
    AstNode *subscript;
};

struct AstNodeExternBlock {
    ZigList<AstNode *> *directives;
    ZigList<AstNode *> fn_decls;
};

struct AstNodeDirective {
    Buf name;
    Buf param;
};

struct AstNodeRootExportDecl {
    Buf type;
    Buf name;
    ZigList<AstNode *> *directives;
};

struct AstNodeCastExpr {
    AstNode *expr;
    AstNode *type;
};

enum PrefixOp {
    PrefixOpInvalid,
    PrefixOpBoolNot,
    PrefixOpBinNot,
    PrefixOpNegation,
};

struct AstNodePrefixOpExpr {
    PrefixOp prefix_op;
    AstNode *primary_expr;
};

struct AstNodeUse {
    Buf path;
    ZigList<AstNode *> *directives;
};

struct AstNodeIfExpr {
    AstNode *condition;
    AstNode *then_block;
    AstNode *else_node; // null, block node, or other if expr node
};

struct AstNodeLabel {
    Buf name;
};

struct AstNodeGoto {
    Buf name;
};

struct AsmOutput {
    Buf asm_symbolic_name;
    Buf constraint;
    Buf variable_name;
};

struct AsmInput {
    Buf asm_symbolic_name;
    Buf constraint;
    AstNode *expr;
};

struct SrcPos {
    int line;
    int column;
};

struct AstNodeAsmExpr {
    bool is_volatile;
    Buf asm_template;
    ZigList<SrcPos> offset_map;
    ZigList<AsmToken> token_list;
    ZigList<AsmOutput*> output_list;
    ZigList<AsmInput*> input_list;
    ZigList<Buf*> clobber_list;
};

struct AstNode {
    enum NodeType type;
    int line;
    int column;
    CodeGenNode *codegen_node;
    ImportTableEntry *owner;
    union {
        AstNodeRoot root;
        AstNodeRootExportDecl root_export_decl;
        AstNodeFnDef fn_def;
        AstNodeFnDecl fn_decl;
        AstNodeFnProto fn_proto;
        AstNodeType type;
        AstNodeParamDecl param_decl;
        AstNodeBlock block;
        AstNodeReturnExpr return_expr;
        AstNodeVariableDeclaration variable_declaration;
        AstNodeBinOpExpr bin_op_expr;
        AstNodeExternBlock extern_block;
        AstNodeDirective directive;
        AstNodeCastExpr cast_expr;
        AstNodePrefixOpExpr prefix_op_expr;
        AstNodeFnCallExpr fn_call_expr;
        AstNodeArrayAccessExpr array_access_expr;
        AstNodeUse use;
        AstNodeIfExpr if_expr;
        AstNodeLabel label;
        AstNodeGoto go_to;
        AstNodeAsmExpr asm_expr;
        Buf number;
        Buf string;
        Buf symbol;
        bool bool_literal;
    } data;
};

enum AsmTokenId {
    AsmTokenIdTemplate,
    AsmTokenIdPercent,
    AsmTokenIdVar,
};

struct AsmToken {
    enum AsmTokenId id;
    int start;
    int end;
};

__attribute__ ((format (printf, 2, 3)))
void ast_token_error(Token *token, const char *format, ...);


// This function is provided by generated code, generated by parsergen.cpp
AstNode * ast_parse(Buf *buf, ZigList<Token> *tokens, ImportTableEntry *owner, ErrColor err_color);

const char *node_type_str(NodeType node_type);

void ast_print(AstNode *node, int indent);

#endif
