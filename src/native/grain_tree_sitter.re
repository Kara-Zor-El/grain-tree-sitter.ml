open Tree_sitter;

module Backend = {
  type parser = Parser.t;
  type tree = Tree.t;
  type node = Node.t;

  module Parser = {
    type t = parser;

    let create = () => {
      let language = Tree_sitter_grain.language();
      Parser.create(language);
    };

    let parse_string = (~old=?, parser, source) =>
      switch (old) {
      | None => Parser.parse_string(parser, source)
      | Some(old_tree) => Parser.parse_string(~old=old_tree, parser, source)
      };
  };

  module Tree = {
    type t = tree;

    let root_node = Tree.root_node;
    let copy = Tree.copy;

    let edit = (tree, edit: Edit.t) => {
      Tree.edit(
        tree,
        ~start_byte=edit.start_byte,
        ~old_end_byte=edit.old_end_byte,
        ~new_end_byte=edit.new_end_byte,
        ~start_point={
          Tree_sitter.row: edit.start_point.row,
          Tree_sitter.column: edit.start_point.column,
        },
        ~old_end_point={
          Tree_sitter.row: edit.old_end_point.row,
          Tree_sitter.column: edit.old_end_point.column,
        },
        ~new_end_point={
          Tree_sitter.row: edit.new_end_point.row,
          Tree_sitter.column: edit.new_end_point.column,
        },
      );
    };
  };

  module Node = {
    type t = node;

    let kind = Node.kind;
    let start_byte = Node.start_byte;
    let end_byte = Node.end_byte;
    let start_point = node => {
      let point = Node.start_point(node);
      (point.row, point.column);
    };
    let end_point = node => {
      let point = Node.end_point(node);
      (point.row, point.column);
    };
    let parent = Node.parent;
    let named_child = Node.named_child;
    let named_child_count = Node.named_child_count;
    let child_by_field_name = Node.child_by_field_name;
    let named_descendant_for_point = (node, ~row, ~column) => {
      let point = {
        row,
        column,
      };
      Node.named_descendant_for_point_range(node, ~start=point, ~end_=point);
    };
    let descendant_for_point = (node, ~row, ~column) => {
      let point = {
        row,
        column,
      };
      Node.descendant_for_point_range(node, ~start=point, ~end_=point);
    };
  };
};

include Grain_tree_sitter_core.Grain_tree_sitter;
include Grain_tree_sitter_core.Grain_tree_sitter.Make(Backend);

module InnerTree = Tree;
let inner_node_at_point = node_at_point;
let inner_node_text = node_text;

module Tree = {
  type node = Node.t;
  type t = parse_tree;

  let source = tree => tree.source;
  let root = tree => InnerTree.root_node(tree.tree);
  let node_at_point = (tree, pos) => inner_node_at_point(tree.tree, pos);
  let node_text = (tree, node) => inner_node_text(tree.source, node);
  let node_kind = Node.kind;
  let node_start_byte = Node.start_byte;
  let node_end_byte = Node.end_byte;
  let node_start_point = Node.start_point;
  let node_end_point = Node.end_point;
  let node_parent = Node.parent;
  let node_named_child = Node.named_child;
  let node_named_child_count = Node.named_child_count;
  let node_child_by_field_name = Node.child_by_field_name;
  let node_descendant_for_point = (tree, pos: position) => {
    let root = InnerTree.root_node(tree.tree);
    Node.descendant_for_point(
      root,
      ~row=pos.line,
      ~column=pos.character,
    );
  };
  let ancestor = ancestor;
  let node_or_ancestor = node_or_ancestor;
  let cursor_before = cursor_before;
  let cursor_after = cursor_after;
  let cursor_inside = cursor_inside;
  let cursor_in_node = cursor_in_node;
};

let on_ready = f => f();
