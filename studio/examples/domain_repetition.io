
;; Domain repetition example.
;;
;; Domain repetition is a way to cheaply duplicate an object up to infinity.
;; Instead of unioning translated objects, we can achieve the same result by
;; transforming the coordinates using a modulo/floor-remainder operation.
;; This results in duplication of shapes with constant time complexity with
;; regards to the number of duplications, as opposed to the standard union
;; approach, the time complexity of which scales linearly.
;;
;; This example demonstrates domain repetition in the cylindrical coordinate
;; system as well in the cartesian coordinate system.
;;
;; Original involute spur gear source code with full explanation:
;; https://github.com/mkeeter/mpr/blob/eb63defa00239571984e579039166ea575da0dad/gui/examples/involute_gear.io

(set-quality! 11)
(set-resolution! 100)
(set-bounds! [-5 -5 -5] [5 5 5])

(define %pi 3.14159265359)
(define %2pi (* 2 %pi))

;; Creates a function that successively mirrors the interval [`a`, `b`] `rep`-times.
;; The direction in which the interval is successively mirrored is determined by the sign of `rep`.
;; The behavior at the end of repetitions is determined by the `clamp` argument.
;; If set to `#t`, the function evaluated past the repetition range will evaluate to one of the
;; interval boundary values `a` or `b`. If set to `#f`, the value is not clamped and
;; will continue as an affine function extending from the last repetition.
;;
;; See this graph for a nice visualization:
;; https://www.desmos.com/calculator/9jz4sbduuy
;;
;; Arguments:
;; `a`: Real number, start of the interval to repeat.
;; `b`: Real number, end of the interval to repeat.
;; `rep`: Integer, number of successive, mirrored repetitions of the interval with sign deciding the direction.
;;        `-inf.0` and `+inf.0` can be used to repeat to infinity in the negative and positive direction, respectively.
;; `clamp`: Boolean, whether to clamp the value at the edge of the successive repetitions or not.
;;
;; See `make-mirroring-repeater-inf`.
;;
;; Implementation notes:
;; Currently, the function `modulo` is used as it seems to have the equivalent functionality
;; within the shape definition scope as `floor-remainder` outside of the shape definition
;; scope.
(define (make-mirroring-repeater a b rep clamp)
  (let*
    ((M (- b a)))
    (cond
      ((> rep 0) (let*
        ((f (lambda (x) (+ M a (- (abs (- (modulo (max 0 (min (- x a) (* M rep))) (* 2 M)) M)))))))
        (cond
          (clamp f)
          ((equal? rep +inf.0) (lambda (x) (min x (f x))))
          ((odd? rep) (lambda (x) (min x (max (f x) (- x (* M (- rep 1)))))))
          (else (lambda (x) (min x (min (f x) (+ (* 2 a) (* M rep) (- x)))))))))
      ((< rep 0) (let*
        ((f (lambda (x) (+ M a (- (abs (- (modulo (max (* M (+ rep 1)) (min (- x a) M)) (* 2 M)) M)))))))
        (cond
          (clamp f)
          ((equal? rep -inf.0) (lambda (x) (max x (f x))))
          ((odd? rep) (lambda (x) (max x (min (f x) (- x (* M (+ rep 1)))))))
          (else (lambda (x) (max x (max (f x) (+ (* 2 a) (* M (+ rep 2)) (- x)))))))))
      (else (lambda (x) (nan)))
    )
  )
)

;; Creates a function that successively mirrors the interval [`a`, `b`] from negative infinity to positive infinity.
;;
;; Arguments:
;; `a`: Real number, start of the interval to repeat.
;; `b`: Real number, end of the interval to repeat.
;;
;; See `make-mirroring-repeater`.
(define (make-mirroring-repeater-inf a b)
  (let*
    ((M (- b a)))
    (lambda (x) (+ M a (- (abs (- (modulo (- x a) (* 2 M)) M)))))
  )
)

(define (repeat-x shape a b rep clamp)
    (let* ((mirroring-repeater (make-mirroring-repeater a b rep clamp)))
      (remap-shape (shape x y z)
        (mirroring-repeater x)
        y
        z
      )
    )
  )

(define (repeat-y shape a b rep clamp)
    (let* ((mirroring-repeater (make-mirroring-repeater a b rep clamp)))
      (remap-shape (shape x y z)
        x
        (mirroring-repeater y)
        z
      )
    )
  )

(define (repeat-z shape a b rep clamp)
    (let* ((mirroring-repeater (make-mirroring-repeater a b rep clamp)))
      (remap-shape (shape x y z)
        x
        y
        (mirroring-repeater z)
      )
    )
  )

;; Repeats the angle interval [-offset, -offset + 2 * Pi / rep] `rep` times around the origin.
(define (repeat-angular-xy shape offset rep)
    (let* (
        (a offset)
        (b (+ offset (/ %2pi rep)))
        (mirroring-repeater (make-mirroring-repeater-inf a b))
        (map-coords (lambda (x' y' out-index)
          (let* (
              (r (norm [x' y']))
              (phi (atan y' x'))
              (phi (mirroring-repeater phi))
            )
            (case out-index
              ((0) (* r (cos phi)))
              ((1) (* r (sin phi)))
              (else (nan)))
        )))
      )
      (remap-shape (shape x y z)
        (map-coords x y 0)
        (map-coords x y 1)
        z
      )
    )
  )

;; pitch radius, pressure angle (radians), diametral pitch
(define (gear r pa p)
    ;; Round to the nearest number of teeth
  (let* (
    (n (round (* r p 2)))

    ;; Updated pitch radius based on number of teeth
    (r (/ n p 2))

    (rb (* r (cos pa))) ;; base circle radius
    (a (/ p)) ;; addendum
    (d (/ 1.157 p)) ;; dedendum
    (ro (+ r a)) ;; outer radius
    (rr (- r d)) ;; inner radius

    (t (sqrt (- (/ (square r) (square rb)) 1)))
    (rot (+ (atan (- (sin t) (* t (cos t)))
                         (+ (cos t) (* t (sin t)))) (/ pi 2 n)))

    (tooth (lambda-shape (x y z)
      (let* ((r2 (+ (square x) (square y)))
             (r (sqrt r2)))
      (- (sqrt (max 0 (- r2 (square rb))))
         (* rb (+ (atan (/ y x)) (acos (min 1 (max -1 (/ rb r)))) rot))))))

    (tooth (intersection tooth
        (reflect-y tooth)
        (lambda-shape (x y z) (- x))))

    ;; Teeth without domain repetition:
    ; (teeth
    ;     (apply union
    ;         (map (lambda (i) (rotate-z tooth (* 2 pi (/ i n))))
    ;              (iota n)))))

    ;; Teeth with domain repetition:
    (teeth
      (repeat-angular-xy tooth (- (/ %pi n)) n))
  )
  (scale-xyz
    (intersection (circle ro) (union teeth (circle rr)))
    [0.3 0.3 0.1])
))

(sequence
  (gear 10.25 0.3 10)
  (difference(circle 0.1))
  (extrude-z -0.07 0.07)
  (repeat-z -0.2 0.2 3 #f))

