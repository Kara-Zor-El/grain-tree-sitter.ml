include Grain_tree_sitter_core.Grain_tree_sitter;
include Grain_tree_sitter_core.Grain_tree_sitter.Make(Tree_sitter_web);

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

external on_ready: (unit => unit) => unit = "grain_ts_on_ready";
