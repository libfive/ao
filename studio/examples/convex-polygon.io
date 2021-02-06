;; This file is part of the load.io demonstration


;; Counts windings of a polygon
;; `vs` is a list of 2d vertices
;; example:
;; ```
;; $ (polygon-windings (list [0 0] [0 1] [2 1] [1 0])) 
;; 3
;; ```
;; Source:
;; https://www.element84.com/blog/determining-the-winding-of-a-polygon-given-as-a-set-of-ordered-points
(define (polygon-windings vs)
  (define (compute-angle e1 e2) (* (- (.x e2) (.x e1)) (+ (.y e2) (.y e1))))
  (define (list-rotate vs) (append (cdr vs) (list (car vs))))
  (if (> 3 (length vs)) (error "polygon must have at least 3 vertices"))
  (fold + 0 (map compute-angle vs (list-rotate vs)))
)


;; Creates a (convex) polygon shape
;; `vs` is a list of 2d vertices
;; `vs` must define a convex polygon. The first and last
;; vertices are connected automaticaly.
;; example:
;; ```
;; $ (convex-polygon (list [0 0] [0 1] [1 1]  [1 0]
;; #<<shape> ...>
;; ```
(define (convex-polygon vs)
  (define (half-plane a b)
    (lambda-shape (x y z)
      (- (* (- (.y b) (.y a)) (- x (.x a)))
         (* (- (.x b) (.x a)) (- y (.y a))))))
  (define (list-rotate vs) (append (cdr vs) (list (car vs))))
  (define (reduce proc vs) (fold proc (car vs) (cdr vs)))
  (define (proc v1 v2)
    (if (> 0 (polygon-windings vs))
      (half-plane v1 v2)
      (half-plane v2 v1)
    )
  )
  (reduce intersection (map proc vs (list-rotate vs)))
)
