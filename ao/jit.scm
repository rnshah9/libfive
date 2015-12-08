(define-module (ao jit))

(use-modules (system foreign))
(load-extension "libfab" "fab_init")

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;; This is our local store.  When populated, it's an opaque pointer to a
;; Store object (created in C++ with make-store)
(define store #nil)

;; Converts the given argument to a token
;; Based on argument type, the following behavior occurs
;; - A number is converted to constant
;; - A symbol and further arguments is converted to operator
(define-public (make-token a . args)
  (cond ((= 0 (length args))
            (cond ((pointer? a) a)
                  ((number? a) (token-const store a))
                  (else (error "Failed to construct token" a))))
        (else (token-op store a (map make-token args)))))

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

(define-public (jit f)
  (let ((args (car (procedure-minimum-arity f))))
    (set! store (make-store))
    (let ((x (token-x store))
          (y (token-y store))
          (z (token-y store)))
    (cond ((= 2 args) (make-token (f x y)))
          ((= 3 args) (make-token (f x y z)))
          (else (error "Invalid function arity" f))))))

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
