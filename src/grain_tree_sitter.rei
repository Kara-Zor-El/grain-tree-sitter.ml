type parse_tree;

type position = {
  line: int,
  character: int,
};

let parse: string => parse_tree;
let reparse: (~old: parse_tree, string) => parse_tree;
let cached_tree:
  (~parse_trees: Hashtbl.t('key, (int, parse_tree)), 'key, string) =>
  parse_tree;
let on_ready: (unit => unit) => unit;

module Tree: {
  type node;
  type t = parse_tree;

  let source: t => string;
  let root: t => node;
  let node_at_point: (t, position) => option(node);
  let node_text: (t, node) => string;
  let node_kind: node => string;
  let node_start_byte: node => int;
  let node_end_byte: node => int;
  let node_start_point: node => (int, int);
  let node_end_point: node => (int, int);
  let node_parent: node => option(node);
  let node_named_child: (node, int) => option(node);
  let node_named_child_count: node => int;
  let node_child_by_field_name: (node, string) => option(node);
  let node_descendant_for_point: (t, position) => option(node);
  let ancestor: (node, node => bool) => option(node);
  let node_or_ancestor: (node, string) => option(node);
  let cursor_before: (position, int, int) => bool;
  let cursor_after: (position, int, int) => bool;
  let cursor_inside: (position, node) => bool;
  let cursor_in_node: (position, node, node) => bool;
};
