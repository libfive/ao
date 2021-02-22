#|
Guile bindings to the libfive CAD kernel

DO NOT EDIT BY HAND!
This file is automatically generated from libfive/stdlib/stdlib.h

It was last generated on 2021-02-22 08:59:46 by user mkeeter
|#

(define-module (libfive stdlib transforms))
(use-modules (system foreign) (libfive lib) (libfive kernel) (libfive vec))

(define ffi_move (pointer->procedure '*
  (dynamic-func "move" stdlib)
  (list '* (list '* '* '*))))
(define* (move t offset)
  "move t offset
  Moves the given shape in 2D or 3D space"
  (ptr->shape (ffi_move
    (shape->ptr (ensure-shape t))
    (vec3->tvec3 offset))))
(export move)

(define ffi_reflect-x (pointer->procedure '*
  (dynamic-func "reflect_x" stdlib)
  (list '* '*)))
(define* (reflect-x t #:optional (x0 0))
  "reflect-x t #:optional (x0 0)
  Reflects a shape about the x origin or an optional offset"
  (ptr->shape (ffi_reflect-x
    (shape->ptr (ensure-shape t))
    (shape->ptr (ensure-shape x0)))))
(export reflect-x)

(define ffi_reflect-y (pointer->procedure '*
  (dynamic-func "reflect_y" stdlib)
  (list '* '*)))
(define* (reflect-y t #:optional (y0 0))
  "reflect-y t #:optional (y0 0)
  Reflects a shape about the y origin or an optional offset"
  (ptr->shape (ffi_reflect-y
    (shape->ptr (ensure-shape t))
    (shape->ptr (ensure-shape y0)))))
(export reflect-y)

(define ffi_reflect-z (pointer->procedure '*
  (dynamic-func "reflect_z" stdlib)
  (list '* '*)))
(define* (reflect-z t #:optional (z0 0))
  "reflect-z t #:optional (z0 0)
  Reflects a shape about the z origin or an optional offset"
  (ptr->shape (ffi_reflect-z
    (shape->ptr (ensure-shape t))
    (shape->ptr (ensure-shape z0)))))
(export reflect-z)

(define ffi_reflect-xy (pointer->procedure '*
  (dynamic-func "reflect_xy" stdlib)
  (list '*)))
(define* (reflect-xy t)
  "reflect-xy t
  Reflects a shape about the plane X=Y"
  (ptr->shape (ffi_reflect-xy
    (shape->ptr (ensure-shape t)))))
(export reflect-xy)

(define ffi_reflect-yz (pointer->procedure '*
  (dynamic-func "reflect_yz" stdlib)
  (list '*)))
(define* (reflect-yz t)
  "reflect-yz t
  Reflects a shape about the plane Y=Z"
  (ptr->shape (ffi_reflect-yz
    (shape->ptr (ensure-shape t)))))
(export reflect-yz)

(define ffi_reflect-xz (pointer->procedure '*
  (dynamic-func "reflect_xz" stdlib)
  (list '*)))
(define* (reflect-xz t)
  "reflect-xz t
  Reflects a shape about the plane X=Z"
  (ptr->shape (ffi_reflect-xz
    (shape->ptr (ensure-shape t)))))
(export reflect-xz)

(define ffi_symmetric-x (pointer->procedure '*
  (dynamic-func "symmetric_x" stdlib)
  (list '*)))
(define* (symmetric-x t)
  "symmetric-x t
  Clips the given shape at the x origin, then duplicates the remaining
  shape reflected on the other side of the origin"
  (ptr->shape (ffi_symmetric-x
    (shape->ptr (ensure-shape t)))))
(export symmetric-x)

(define ffi_symmetric-y (pointer->procedure '*
  (dynamic-func "symmetric_y" stdlib)
  (list '*)))
(define* (symmetric-y t)
  "symmetric-y t
  Clips the given shape at the y origin, then duplicates the remaining
  shape reflected on the other side of the origin"
  (ptr->shape (ffi_symmetric-y
    (shape->ptr (ensure-shape t)))))
(export symmetric-y)

(define ffi_symmetric-z (pointer->procedure '*
  (dynamic-func "symmetric_z" stdlib)
  (list '*)))
(define* (symmetric-z t)
  "symmetric-z t
  Clips the given shape at the z origin, then duplicates the remaining
  shape reflected on the other side of the origin"
  (ptr->shape (ffi_symmetric-z
    (shape->ptr (ensure-shape t)))))
(export symmetric-z)

(define ffi_scale-x (pointer->procedure '*
  (dynamic-func "scale_x" stdlib)
  (list '* '* '*)))
(define* (scale-x t sx #:optional (x0 0))
  "scale-x t sx #:optional (x0 0)
  Scales a shape by sx on the x axis about 0 or an optional offset"
  (ptr->shape (ffi_scale-x
    (shape->ptr (ensure-shape t))
    (shape->ptr (ensure-shape sx))
    (shape->ptr (ensure-shape x0)))))
(export scale-x)

(define ffi_scale-y (pointer->procedure '*
  (dynamic-func "scale_y" stdlib)
  (list '* '* '*)))
(define* (scale-y t sy #:optional (y0 0))
  "scale-y t sy #:optional (y0 0)
  Scales a shape by sx on the x axis about 0 or an optional offset"
  (ptr->shape (ffi_scale-y
    (shape->ptr (ensure-shape t))
    (shape->ptr (ensure-shape sy))
    (shape->ptr (ensure-shape y0)))))
(export scale-y)

(define ffi_scale-z (pointer->procedure '*
  (dynamic-func "scale_z" stdlib)
  (list '* '* '*)))
(define* (scale-z t sz #:optional (z0 0))
  "scale-z t sz #:optional (z0 0)
  Scales a shape by sx on the x axis about 0 or an optional offset"
  (ptr->shape (ffi_scale-z
    (shape->ptr (ensure-shape t))
    (shape->ptr (ensure-shape sz))
    (shape->ptr (ensure-shape z0)))))
(export scale-z)

(define ffi_scale-xyz (pointer->procedure '*
  (dynamic-func "scale_xyz" stdlib)
  (list '* (list '* '* '*) (list '* '* '*))))
(define* (scale-xyz t s center)
  "scale-xyz t s center
  Scales a shape on all three axes, about 0 or an optional offset"
  (ptr->shape (ffi_scale-xyz
    (shape->ptr (ensure-shape t))
    (vec3->tvec3 s)
    (vec3->tvec3 center))))
(export scale-xyz)

(define ffi_rotate-x (pointer->procedure '*
  (dynamic-func "rotate_x" stdlib)
  (list '* '* (list '* '* '*))))
(define* (rotate-x t angle #:optional (center #[0 0 0]))
  "rotate-x t angle #:optional (center #[0 0 0])
  Rotate the given shape by an angle in radians
  The center of rotation is [0 0 0] or specified by the optional argument"
  (ptr->shape (ffi_rotate-x
    (shape->ptr (ensure-shape t))
    (shape->ptr (ensure-shape angle))
    (vec3->tvec3 center))))
(export rotate-x)

(define ffi_rotate-y (pointer->procedure '*
  (dynamic-func "rotate_y" stdlib)
  (list '* '* (list '* '* '*))))
(define* (rotate-y t angle #:optional (center #[0 0 0]))
  "rotate-y t angle #:optional (center #[0 0 0])
  Rotate the given shape by an angle in radians
  The center of rotation is [0 0 0] or specified by the optional argument"
  (ptr->shape (ffi_rotate-y
    (shape->ptr (ensure-shape t))
    (shape->ptr (ensure-shape angle))
    (vec3->tvec3 center))))
(export rotate-y)

(define ffi_rotate-z (pointer->procedure '*
  (dynamic-func "rotate_z" stdlib)
  (list '* '* (list '* '* '*))))
(define* (rotate-z t angle #:optional (center #[0 0 0]))
  "rotate-z t angle #:optional (center #[0 0 0])
  Rotate the given shape by an angle in radians
  The center of rotation is [0 0 0] or specified by the optional argument"
  (ptr->shape (ffi_rotate-z
    (shape->ptr (ensure-shape t))
    (shape->ptr (ensure-shape angle))
    (vec3->tvec3 center))))
(export rotate-z)

(define ffi_taper-x-y (pointer->procedure '*
  (dynamic-func "taper_x_y" stdlib)
  (list '* (list '* '*) '* '* '*)))
(define* (taper-x-y shape base h scale #:optional (base_scale 1))
  "taper-x-y shape base h scale #:optional (base_scale 1)
  Tapers a shape along the x axis as a function of y
  width = base-scale at base
  width = scale at base + [0 h]"
  (ptr->shape (ffi_taper-x-y
    (shape->ptr (ensure-shape shape))
    (vec2->tvec2 base)
    (shape->ptr (ensure-shape h))
    (shape->ptr (ensure-shape scale))
    (shape->ptr (ensure-shape base_scale)))))
(export taper-x-y)

(define ffi_taper-xy-z (pointer->procedure '*
  (dynamic-func "taper_xy_z" stdlib)
  (list '* (list '* '* '*) '* '* '*)))
(define* (taper-xy-z shape base height scale #:optional (base_scale 1))
  "taper-xy-z shape base height scale #:optional (base_scale 1)
  Tapers a shape in the xy plane as a function of z
  width = base-scale at base
  width = scale at base + [0 0 height]"
  (ptr->shape (ffi_taper-xy-z
    (shape->ptr (ensure-shape shape))
    (vec3->tvec3 base)
    (shape->ptr (ensure-shape height))
    (shape->ptr (ensure-shape scale))
    (shape->ptr (ensure-shape base_scale)))))
(export taper-xy-z)

(define ffi_shear-x-y (pointer->procedure '*
  (dynamic-func "shear_x_y" stdlib)
  (list '* (list '* '*) '* '* '*)))
(define* (shear-x-y t base height offset #:optional (base_offset 0))
  "shear-x-y t base height offset #:optional (base_offset 0)
  Shears a shape on the x axis as a function of y
  offset = base-offset at base.y
  offset = offset = base.y + h"
  (ptr->shape (ffi_shear-x-y
    (shape->ptr (ensure-shape t))
    (vec2->tvec2 base)
    (shape->ptr (ensure-shape height))
    (shape->ptr (ensure-shape offset))
    (shape->ptr (ensure-shape base_offset)))))
(export shear-x-y)

(define ffi_repel (pointer->procedure '*
  (dynamic-func "repel" stdlib)
  (list '* (list '* '* '*) '* '*)))
(define* (repel shape locus radius #:optional (exaggerate 1))
  "repel shape locus radius #:optional (exaggerate 1)
  Repels the shape away from a point based upon a radius r,
  with optional exaggeration"
  (ptr->shape (ffi_repel
    (shape->ptr (ensure-shape shape))
    (vec3->tvec3 locus)
    (shape->ptr (ensure-shape radius))
    (shape->ptr (ensure-shape exaggerate)))))
(export repel)

(define ffi_repel-x (pointer->procedure '*
  (dynamic-func "repel_x" stdlib)
  (list '* (list '* '* '*) '* '*)))
(define* (repel-x shape locus radius #:optional (exaggerate 1))
  "repel-x shape locus radius #:optional (exaggerate 1)
  Repels the shape away from a YZ plane based upon a radius r,
  with optional exaggeration"
  (ptr->shape (ffi_repel-x
    (shape->ptr (ensure-shape shape))
    (vec3->tvec3 locus)
    (shape->ptr (ensure-shape radius))
    (shape->ptr (ensure-shape exaggerate)))))
(export repel-x)

(define ffi_repel-y (pointer->procedure '*
  (dynamic-func "repel_y" stdlib)
  (list '* (list '* '* '*) '* '*)))
(define* (repel-y shape locus radius #:optional (exaggerate 1))
  "repel-y shape locus radius #:optional (exaggerate 1)
  Repels the shape away from a XZ plane based upon a radius r,
  with optional exaggeration"
  (ptr->shape (ffi_repel-y
    (shape->ptr (ensure-shape shape))
    (vec3->tvec3 locus)
    (shape->ptr (ensure-shape radius))
    (shape->ptr (ensure-shape exaggerate)))))
(export repel-y)

(define ffi_repel-z (pointer->procedure '*
  (dynamic-func "repel_z" stdlib)
  (list '* (list '* '* '*) '* '*)))
(define* (repel-z shape locus radius #:optional (exaggerate 1))
  "repel-z shape locus radius #:optional (exaggerate 1)
  Repels the shape away from a XY plane based upon a radius r,
  with optional exaggeration"
  (ptr->shape (ffi_repel-z
    (shape->ptr (ensure-shape shape))
    (vec3->tvec3 locus)
    (shape->ptr (ensure-shape radius))
    (shape->ptr (ensure-shape exaggerate)))))
(export repel-z)

(define ffi_repel-xy (pointer->procedure '*
  (dynamic-func "repel_xy" stdlib)
  (list '* (list '* '* '*) '* '*)))
(define* (repel-xy shape locus radius #:optional (exaggerate 1))
  "repel-xy shape locus radius #:optional (exaggerate 1)
  Repels the shape away from line parallel to the Z axis,
  with a particular radius and optional exaggeration"
  (ptr->shape (ffi_repel-xy
    (shape->ptr (ensure-shape shape))
    (vec3->tvec3 locus)
    (shape->ptr (ensure-shape radius))
    (shape->ptr (ensure-shape exaggerate)))))
(export repel-xy)

(define ffi_repel-yz (pointer->procedure '*
  (dynamic-func "repel_yz" stdlib)
  (list '* (list '* '* '*) '* '*)))
(define* (repel-yz shape locus radius #:optional (exaggerate 1))
  "repel-yz shape locus radius #:optional (exaggerate 1)
  Repels the shape away from line parallel to the X axis,
  with a particular radius and optional exaggeration"
  (ptr->shape (ffi_repel-yz
    (shape->ptr (ensure-shape shape))
    (vec3->tvec3 locus)
    (shape->ptr (ensure-shape radius))
    (shape->ptr (ensure-shape exaggerate)))))
(export repel-yz)

(define ffi_repel-xz (pointer->procedure '*
  (dynamic-func "repel_xz" stdlib)
  (list '* (list '* '* '*) '* '*)))
(define* (repel-xz shape locus radius #:optional (exaggerate 1))
  "repel-xz shape locus radius #:optional (exaggerate 1)
  Repels the shape away from line parallel to the Y axis,
  with a particular radius and optional exaggeration"
  (ptr->shape (ffi_repel-xz
    (shape->ptr (ensure-shape shape))
    (vec3->tvec3 locus)
    (shape->ptr (ensure-shape radius))
    (shape->ptr (ensure-shape exaggerate)))))
(export repel-xz)

(define ffi_attract (pointer->procedure '*
  (dynamic-func "attract" stdlib)
  (list '* (list '* '* '*) '* '*)))
(define* (attract shape locus radius #:optional (exaggerate 1))
  "attract shape locus radius #:optional (exaggerate 1)
  Attracts the shape away from a point based upon a radius r,
  with optional exaggeration"
  (ptr->shape (ffi_attract
    (shape->ptr (ensure-shape shape))
    (vec3->tvec3 locus)
    (shape->ptr (ensure-shape radius))
    (shape->ptr (ensure-shape exaggerate)))))
(export attract)

(define ffi_attract-x (pointer->procedure '*
  (dynamic-func "attract_x" stdlib)
  (list '* (list '* '* '*) '* '*)))
(define* (attract-x shape locus radius #:optional (exaggerate 1))
  "attract-x shape locus radius #:optional (exaggerate 1)
  Attracts the shape away from a YZ plane based upon a radius r,
  with optional exaggeration"
  (ptr->shape (ffi_attract-x
    (shape->ptr (ensure-shape shape))
    (vec3->tvec3 locus)
    (shape->ptr (ensure-shape radius))
    (shape->ptr (ensure-shape exaggerate)))))
(export attract-x)

(define ffi_attract-y (pointer->procedure '*
  (dynamic-func "attract_y" stdlib)
  (list '* (list '* '* '*) '* '*)))
(define* (attract-y shape locus radius #:optional (exaggerate 1))
  "attract-y shape locus radius #:optional (exaggerate 1)
  Attracts the shape away from a XZ plane based upon a radius r,
  with optional exaggeration"
  (ptr->shape (ffi_attract-y
    (shape->ptr (ensure-shape shape))
    (vec3->tvec3 locus)
    (shape->ptr (ensure-shape radius))
    (shape->ptr (ensure-shape exaggerate)))))
(export attract-y)

(define ffi_attract-z (pointer->procedure '*
  (dynamic-func "attract_z" stdlib)
  (list '* (list '* '* '*) '* '*)))
(define* (attract-z shape locus radius #:optional (exaggerate 1))
  "attract-z shape locus radius #:optional (exaggerate 1)
  Attracts the shape away from a XY plane based upon a radius r,
  with optional exaggeration"
  (ptr->shape (ffi_attract-z
    (shape->ptr (ensure-shape shape))
    (vec3->tvec3 locus)
    (shape->ptr (ensure-shape radius))
    (shape->ptr (ensure-shape exaggerate)))))
(export attract-z)

(define ffi_attract-xy (pointer->procedure '*
  (dynamic-func "attract_xy" stdlib)
  (list '* (list '* '* '*) '* '*)))
(define* (attract-xy shape locus radius #:optional (exaggerate 1))
  "attract-xy shape locus radius #:optional (exaggerate 1)
  Attracts the shape away from line parallel to the Z axis,
  with a particular radius and optional exaggeration"
  (ptr->shape (ffi_attract-xy
    (shape->ptr (ensure-shape shape))
    (vec3->tvec3 locus)
    (shape->ptr (ensure-shape radius))
    (shape->ptr (ensure-shape exaggerate)))))
(export attract-xy)

(define ffi_attract-yz (pointer->procedure '*
  (dynamic-func "attract_yz" stdlib)
  (list '* (list '* '* '*) '* '*)))
(define* (attract-yz shape locus radius #:optional (exaggerate 1))
  "attract-yz shape locus radius #:optional (exaggerate 1)
  Attracts the shape away from line parallel to the X axis,
  with a particular radius and optional exaggeration"
  (ptr->shape (ffi_attract-yz
    (shape->ptr (ensure-shape shape))
    (vec3->tvec3 locus)
    (shape->ptr (ensure-shape radius))
    (shape->ptr (ensure-shape exaggerate)))))
(export attract-yz)

(define ffi_attract-xz (pointer->procedure '*
  (dynamic-func "attract_xz" stdlib)
  (list '* (list '* '* '*) '* '*)))
(define* (attract-xz shape locus radius #:optional (exaggerate 1))
  "attract-xz shape locus radius #:optional (exaggerate 1)
  Attracts the shape away from line parallel to the Y axis,
  with a particular radius and optional exaggeration"
  (ptr->shape (ffi_attract-xz
    (shape->ptr (ensure-shape shape))
    (vec3->tvec3 locus)
    (shape->ptr (ensure-shape radius))
    (shape->ptr (ensure-shape exaggerate)))))
(export attract-xz)

(define ffi_revolve-y (pointer->procedure '*
  (dynamic-func "revolve_y" stdlib)
  (list '* '*)))
(define* (revolve-y shape #:optional (x0 0))
  "revolve-y shape #:optional (x0 0)
  Revolves a 2D (XY) shape about a line parallel to the Y axis with the
  given x value"
  (ptr->shape (ffi_revolve-y
    (shape->ptr (ensure-shape shape))
    (shape->ptr (ensure-shape x0)))))
(export revolve-y)

(define ffi_twirl-x (pointer->procedure '*
  (dynamic-func "twirl_x" stdlib)
  (list '* '* '* (list '* '* '*))))
(define* (twirl-x shape amount radius #:optional (center #[0 0 0]))
  "twirl-x shape amount radius #:optional (center #[0 0 0])
  Twirls the shape in the x axis about the (optional) center point"
  (ptr->shape (ffi_twirl-x
    (shape->ptr (ensure-shape shape))
    (shape->ptr (ensure-shape amount))
    (shape->ptr (ensure-shape radius))
    (vec3->tvec3 center))))
(export twirl-x)

(define ffi_twirl-axis-x (pointer->procedure '*
  (dynamic-func "twirl_axis_x" stdlib)
  (list '* '* '* (list '* '* '*))))
(define* (twirl-axis-x shape amount radius #:optional (center #[0 0 0]))
  "twirl-axis-x shape amount radius #:optional (center #[0 0 0])
  Twirls the shape in the x axis about the line extending from the
  (optional) center point"
  (ptr->shape (ffi_twirl-axis-x
    (shape->ptr (ensure-shape shape))
    (shape->ptr (ensure-shape amount))
    (shape->ptr (ensure-shape radius))
    (vec3->tvec3 center))))
(export twirl-axis-x)

(define ffi_twirl-y (pointer->procedure '*
  (dynamic-func "twirl_y" stdlib)
  (list '* '* '* (list '* '* '*))))
(define* (twirl-y shape amount radius #:optional (center #[0 0 0]))
  "twirl-y shape amount radius #:optional (center #[0 0 0])
  Twirls the shape in the y axis about the (optional) center point"
  (ptr->shape (ffi_twirl-y
    (shape->ptr (ensure-shape shape))
    (shape->ptr (ensure-shape amount))
    (shape->ptr (ensure-shape radius))
    (vec3->tvec3 center))))
(export twirl-y)

(define ffi_twirl-axis-y (pointer->procedure '*
  (dynamic-func "twirl_axis_y" stdlib)
  (list '* '* '* (list '* '* '*))))
(define* (twirl-axis-y shape amount radius #:optional (center #[0 0 0]))
  "twirl-axis-y shape amount radius #:optional (center #[0 0 0])
  Twirls the shape in the y axis about the line extending from the
  (optional) center point"
  (ptr->shape (ffi_twirl-axis-y
    (shape->ptr (ensure-shape shape))
    (shape->ptr (ensure-shape amount))
    (shape->ptr (ensure-shape radius))
    (vec3->tvec3 center))))
(export twirl-axis-y)

(define ffi_twirl-z (pointer->procedure '*
  (dynamic-func "twirl_z" stdlib)
  (list '* '* '* (list '* '* '*))))
(define* (twirl-z shape amount radius #:optional (center #[0 0 0]))
  "twirl-z shape amount radius #:optional (center #[0 0 0])
  Twirls the shape in the z axis about the (optional) center point"
  (ptr->shape (ffi_twirl-z
    (shape->ptr (ensure-shape shape))
    (shape->ptr (ensure-shape amount))
    (shape->ptr (ensure-shape radius))
    (vec3->tvec3 center))))
(export twirl-z)

(define ffi_twirl-axis-z (pointer->procedure '*
  (dynamic-func "twirl_axis_z" stdlib)
  (list '* '* '* (list '* '* '*))))
(define* (twirl-axis-z shape amount radius #:optional (center #[0 0 0]))
  "twirl-axis-z shape amount radius #:optional (center #[0 0 0])
  Twirls the shape in the z axis about the line extending from the
  (optional) center point"
  (ptr->shape (ffi_twirl-axis-z
    (shape->ptr (ensure-shape shape))
    (shape->ptr (ensure-shape amount))
    (shape->ptr (ensure-shape radius))
    (vec3->tvec3 center))))
(export twirl-axis-z)

(define-public rotate rotate-z)
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; Hand-written override to let move work with vec2
(define move-prev move)
(define-public (move shape v)
  (if (vec2? v) (move-prev shape #[(.x v) (.y v) 0])
                (move-prev shape v)))
(set-procedure-property! move 'documentation
  (procedure-documentation move-prev))
