#|
Guile bindings to the libfive CAD kernel
Copyright (C) 2017  Matt Keeter

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
|#
(use-modules (ice-9 sandbox) (ice-9 textual-ports) (libfive kernel)
             (libfive vec) (rnrs io ports) (system vm frame) (oop goops)
             (srfi srfi-1))

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

(define integer-chars
  (map (lambda (i) (integer->char (+ i (char->integer #\0))))
       (iota 10)))

(define-public vars (make-hash-table))

;; Install a hash reader for every integer, so that #0..., #1..., #2...
;; all call a special handler that tracks the variable position.  This
;; tagging means that we can edit the script later on and change them.
(map (lambda (c)
  (eval `(read-hash-extend ,c
    (lambda (chr port)
      (unget-char port ,c)
      (let* ((var (make-var))
             (line (port-line port))
             (start (port-column port))
             (value (read port))
             (end (port-column port))
             (pos (list line start end)))
        (when (not (number? value))
          (error "Invalid variable literal" value))
        (hash-set! vars (shape-tree-id var) (list '() var value pos))
        var)))
    (interaction-environment)))
  (cons #\- integer-chars))

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

(define* (sandbox-backtrace stack #:optional (port #t))
  "Prints a clipped backtrace of an error stack
  The error stack must be from a call to eval-sandboxed, as the
  location of ice-9/sandbox.scm in the stack is used to filter."
  (define parent-frame (1-
    (let recurse ((i 4))
      (define src (frame-source (stack-ref stack i)))
      (if (and src (equal? (cadr src) "ice-9/sandbox.scm"))
        i (recurse (1+ i))))))
  (define lowest-frame (1+
    (let recurse ((i parent-frame))
      (if (not (frame-source (stack-ref stack i)))
        (recurse (1- i)) i))))
  (let recurse ((i lowest-frame))
    (if (= i parent-frame)
      (format port "~A: ~A\n" (- i lowest-frame)
              ;; The parent frame is always a call in the form
              ;; (eval (actual code we care about) ...), so we
              ;; snip out that actual code
              (car (frame-arguments (stack-ref stack i))))
      (begin
        (let ((s (stack-ref stack i)))
        (format port "~A: ~A\n" (- i lowest-frame)
                (frame-call-representation s)))
        (recurse (1+ i))
        ))))

(define (tag-var-addresses! d addr prev)
  "Recursively walks a clause d, ensuring that vars that have already
  been seen will re-using the same tree (but with a new associated value).

  This is useful because vars are unique clauses, so a tree that's
  [X + $VAR1] is different from [X + $VAR2].  The var reader constructs a
  new variable clause each time the script is evaluated; if we didn't remap
  vars to their previous clauses, then dragging wouldn't work, because the
  top-level tree's clause id would constantly be changing as the vars below
  it changed.

    addr is be a list that contains d's address within a tree

    prev is a map from var addresses to tree objects, which is used
    to detect variables that already exist.
  "
  (cond
    ((list? d)
      (let loop ((e d) (i 0))
        (when (not (nil? e))
          (let ((res (tag-var-addresses! (car e) (cons i addr) prev)))
            (when res
              (list-set! d i res)))
          (loop (cdr e) (1+ i))))
      #f)
    ((vector? d)
      (let loop ((i 0))
        (when (< i (vector-length d))
          (let ((res (tag-var-addresses! (vector-ref d i) (cons i addr) prev)))
            (when res
              (vector-set! d i res)))
          (loop (1+ i))))
       #f)
    ((and (shape? d) (var? d))
      (let ((id (shape-tree-id d))
            (var (hash-ref prev addr)))
        (if var ; If there's a matching previous var, use it
          (let* ((prev (hash-ref vars id))
                 (value (caddr prev))
                 (pos (cadddr prev)))
            (hash-remove! vars id)
            (hash-set! vars (shape-tree-id var) (list addr var value pos))
            var)
          (begin
            (set-car! (hash-ref vars id) addr)
            #f))))
    (else #f)
))

(define-public (eval-sandboxed str)
  (read-enable 'curly-infix)
  (read-disable 'square-brackets)
  (let ((mod (make-sandbox-module sandbox-bindings))
        (in (open-input-string str))
        (failed #f)
        (stack #f)
        (prev-vars (make-hash-table)))
    (hash-map->list (lambda (k v) (hash-set! prev-vars (car v) (cadr v))) vars)
    (hash-clear! vars)
    (eval '(define $bracket-list$ vec-constructor) mod)
    (let loop ((i 0))
      ;; Attempt to read the next clause, storing text location
      (let ((before (cons (port-line in) (port-column in)))
            (clause (catch #t (lambda () (read in))
                              (lambda (key . params)
                                      (set! failed #t)
                                      (list key params))))
            (after (cons (port-line in) (port-column in))))

        (cond
          ;; If we've failed, then record a failed tag
          (failed (list (append (list 'error before after) clause (list ""))))

          ;; If we're at the end of the stream, then sever module
          ;; and return the end of the list
          ((eof-object? clause) (eval-in-sandbox #nil #:module mod))

          ;; Otherwise, attempt to evaluate
          (else
            (tag-var-addresses! clause (list i) prev-vars)
            (let ((result
              (catch #t
                ;; Evaluation thunk
                (lambda () (cons 'valid
                  (call-with-values (lambda ()
                    (eval-in-sandbox clause #:module mod #:sever-module? #f
                                            #:time-limit 10
                                            #:allocation-limit #e10e8))
                    (lambda (. args) args))))
                ;; Error handling thunk
                (lambda (key . params)
                  (set! failed #t)
                  (list 'error before after key params stack))

                ;; Pre-unwind handler to capture stack
                (lambda (key . parameters)
                  (define str (call-with-output-string (lambda (p)
                    (sandbox-backtrace (make-stack #t) p))))
                  (set! stack str))

                )))
              (if (not failed)
                  (cons result (loop (1+ i)))
                  (list result)))))))))

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

(define-public global-bounds #f)
(define-public (set-bounds! lower upper)
  "set-bounds! [xmin ymin zmin] [xmax ymax zmax]
  Sets the global render bounds"
  (when (not (and (vec3? lower) (vec3? upper)))
    (error "Arguments must be vec3"))
  (let ((result (list (.x lower) (.y lower) (.z lower)
                      (.x upper) (.y upper) (.z upper))))
    (if (every number? result)
      (set! global-bounds result)
      (error "All values must be numbers"))))

(define-public global-resolution #f)
(define-public (set-resolution! res)
  "set-resolution! res
  Sets the global render resolution, which is the
  reciprocal of minimum feature size"
  (if (number? res)
    (set! global-resolution res)
    (error "resolution must be a number")))

(define-public global-quality #f)
(define-public (set-quality! q)
  "set-quality! q
  Sets the global render quality, which is a metric
  from 1 to 11 that determines how enthusiastically
  triangles are collapsed in the mesh"
  (when (not (number? q))
    (error "quality must be a number"))
  (when (< q 1)
    (error "quality must be >= 1"))
  (when (> q 11)
    (error "quality must be <= 11"))
  (set! global-quality q))

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

(define (get-bindings mod)
  (cons mod (module-map (lambda (n . a) n) (resolve-interface mod))))

(define-public sandbox-bindings
  (append (list
          (cons '(libfive kernel) libfive-bindings)
          (get-bindings '(libfive vec))
          (get-bindings '(libfive shapes))
          (get-bindings '(libfive csg))
          (get-bindings '(libfive transforms))
          (get-bindings '(libfive text))
          (get-bindings '(libfive util))
          '((libfive sandbox) set-bounds! set-resolution! set-quality!)

          ;; Extra functions from Guile's standard libraries
          '((srfi srfi-1) fold iota)
          '((guile) inexact->exact)
          '((guile) load))

    ;; Default sandbox-safe functions
    all-pure-bindings))
