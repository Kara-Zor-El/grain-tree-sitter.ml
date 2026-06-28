type position = {
  line: int,
  character: int,
};

module type S = {
  type parser;
  type tree;
  type node;

  module Parser: {
    type t = parser;
    let create: unit => t;
    let parse_string: (~old: tree=?, t, string) => tree;
  };

  module Tree: {
    type t = tree;
    let root_node: t => node;
    let copy: t => t;
    let edit: (t, Edit.t) => unit;
  };

  module Node: {
    type t = node;
    let kind: t => string;
    let start_byte: t => int;
    let end_byte: t => int;
    let start_point: t => (int, int);
    let end_point: t => (int, int);
    let parent: t => option(t);
    let named_child: (t, int) => option(t);
    let named_child_count: t => int;
    let child_by_field_name: (t, string) => option(t);
    let named_descendant_for_point: (t, ~row: int, ~column: int) => option(t);
    let descendant_for_point: (t, ~row: int, ~column: int) => option(t);
  };
};

module Make = (Backend: S) => {
  module Parser = Backend.Parser;
  module Tree = Backend.Tree;
  module Node = Backend.Node;

  type parse_tree = {
    source: string,
    tree: Tree.t,
  };

  let parser = ref(None);

  let get_parser = () => {
    switch (parser^) {
    | Some(p) => p
    | None =>
      let p = Parser.create();
      parser := Some(p);
      p;
    };
  };

  let parse = (source: string): parse_tree => {
    let tree = Parser.parse_string(get_parser(), source);
    {
      source,
      tree,
    };
  };

  let reparse = (~old: parse_tree, source: string): parse_tree =>
    if (old.source == source) {
      old;
    } else {
      switch (Edit.compute(old.source, source)) {
      | None => parse(source)
      | Some(edit) =>
        let tree = Tree.copy(old.tree);
        Tree.edit(tree, edit);
        let tree = Parser.parse_string(~old=tree, get_parser(), source);
        {
          source,
          tree,
        };
      };
    };

  let cached_tree = (~parse_trees, key, source: string): parse_tree => {
    let version = Hashtbl.hash(source);
    switch (Hashtbl.find_opt(parse_trees, key)) {
    | Some((cached_version, tree)) when cached_version == version => tree
    | Some((_, old)) =>
      let tree = reparse(~old, source);
      Hashtbl.replace(parse_trees, key, (version, tree));
      tree;
    | None =>
      let tree = parse(source);
      Hashtbl.replace(parse_trees, key, (version, tree));
      tree;
    };
  };

  let node_text = (source, node: Node.t) => {
    let start = Node.start_byte(node);
    let end_ = Node.end_byte(node);
    if (end_ <= start || end_ > String.length(source)) {
      "";
    } else {
      String.sub(source, start, end_ - start);
    };
  };

  let node_at_point = (tree, pos: position) => {
    let root = Tree.root_node(tree);
    Node.named_descendant_for_point(
      root,
      ~row=pos.line,
      ~column=pos.character,
    );
  };

  let rec ancestor = (node, pred) =>
    switch (Node.parent(node)) {
    | None => None
    | Some(parent) =>
      if (pred(parent)) {
        Some(parent);
      } else {
        ancestor(parent, pred);
      }
    };

  let cursor_before = (pos: position, row, column) =>
    pos.line < row || pos.line == row && pos.character < column;

  let cursor_after = (pos: position, row, column) =>
    pos.line > row || pos.line == row && pos.character > column;

  let cursor_inside = (pos: position, node: Node.t) => {
    let (start_row, start_column) = Node.start_point(node);
    let (end_row, end_column) = Node.end_point(node);
    !cursor_before(pos, start_row, start_column)
    && !cursor_after(pos, end_row, end_column);
  };

  let same_node = (left: Node.t, right: Node.t) =>
    Node.start_byte(left) == Node.start_byte(right)
    && Node.end_byte(left) == Node.end_byte(right);

  let rec has_ancestor_node = (node: Node.t, target: Node.t) =>
    switch (Node.parent(node)) {
    | None => false
    | Some(parent) =>
      same_node(parent, target) || has_ancestor_node(parent, target)
    };

  let cursor_in_node = (pos: position, node: Node.t, target: Node.t) =>
    same_node(node, target)
    || cursor_inside(pos, target)
    || has_ancestor_node(node, target);

  let node_or_ancestor = (node, kind) =>
    if (Node.kind(node) == kind) {
      Some(node);
    } else {
      ancestor(node, n => Node.kind(n) == kind);
    };
};
