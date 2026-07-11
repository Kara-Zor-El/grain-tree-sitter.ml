(lang dune 3.8)
(name grain_tree_sitter)
(version 0.1.0)

(generate_opam_files false)

(package
 (name grain_tree_sitter)
 (synopsis "Tree-sitter bindings for Grain")
 (depends
  dune
  reason
  tree-sitter
  tree-sitter-grain))
