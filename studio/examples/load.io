(set-bounds! [-10 -10 -10] [10 10 10])
(set-quality! 8)
(set-resolution! 10)

(load "convex-polygon.io")

(extrude
  (convex-polygon (list [0 0] [0 1] [2 1] [1 0]))
0 1)
